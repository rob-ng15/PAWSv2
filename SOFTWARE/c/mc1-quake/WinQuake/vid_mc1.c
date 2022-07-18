/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// vid_mc1.c -- video driver for MC1

#include "quakedef.h"
#include "d_local.h"
#include "mc1.h"

#include <string.h>

#ifdef __MRISC32__
#include <mr32intrin.h>
#endif

// Video resolution (hardcoded).
#define BASEWIDTH 320
#define BASEHEIGHT ((BASEWIDTH * 9 + 8) / 16)

// Video buffers.
static byte *s_vcp;
static byte *s_palette;
static byte *s_framebuffer;

// Quake working buffers.
static byte *s_vbuffer;
static byte *s_zbuffer;
static byte *s_surfcache;

// Global video state.
extern viddef_t vid;

// Unused.
unsigned short d_8to16table[256];
unsigned d_8to24table[256];

// Pointers for the custom VRAM allocator.
static byte *s_vram_alloc_ptr;
static byte *s_vram_alloc_end;

static void MC1_AllocInit (void)
{
	// We assume that the Quake binary is loaded into XRAM (0x80000000...), or
	// into the "ROM" (0x00000000...) for the simulator, and that it has
	// complete ownership of VRAM (0x40000000...).
	s_vram_alloc_ptr = (byte *)0x40000100;
	s_vram_alloc_end = (byte *)(0x40000000 + GET_MMIO (VRAMSIZE));
}

static byte *MC1_VRAM_Alloc (const size_t bytes)
{
	// Check if there is enough room.
	byte *ptr = s_vram_alloc_ptr;
	byte *new_ptr = ptr + bytes;
	if (new_ptr > s_vram_alloc_end)
		return NULL;

	// Align the next allocation slot to a 32 byte boundary.
	if ((((size_t)new_ptr) & 31) != 0)
		new_ptr += 32U - (((size_t)new_ptr) & 31);
	s_vram_alloc_ptr = new_ptr;

	return ptr;
}

static byte *MC1_Alloc (const size_t bytes)
{
	// Prefer VRAM (for speed).
	byte *ptr = MC1_VRAM_Alloc (bytes);
	if (ptr == NULL)
		ptr = (byte *)malloc (bytes);
	return ptr;
}

static int MC1_IsVRAMPtr (const byte *ptr)
{
	return ptr >= (byte *)0x40000000 && ptr < (byte *)0x80000000;
}

static void MC1_Free (byte *ptr)
{
	// We can't free VRAM pointers.
	if (!MC1_IsVRAMPtr (ptr))
		free (ptr);
}

static void VID_CreateVCP (void)
{
	// Get the native video signal resolution and calculate the scaling factors.
	unsigned native_width = GET_MMIO (VIDWIDTH);
	unsigned native_height = GET_MMIO (VIDHEIGHT);
	unsigned xincr = ((BASEWIDTH - 1) << 16) / (native_width - 1);
	unsigned yincr = ((native_height - 1) << 16) / (BASEHEIGHT - 1);

	// Frame configuraiton.
	unsigned *vcp = (unsigned *)s_vcp;
	*vcp++ = VCP_SETREG (VCR_XINCR, xincr);
	*vcp++ = VCP_SETREG (VCR_CMODE, CMODE_PAL8);
	*vcp++ = VCP_JSR (s_palette);

	// Generate lines.
	unsigned y = 0;
	unsigned addr = (unsigned)s_framebuffer;
	for (int i = 0; i < BASEHEIGHT; ++i)
	{
		if (i == 0)
			*vcp++ = VCP_SETREG (VCR_HSTOP, native_width);
		*vcp++ = VCP_WAITY (y >> 16);
		*vcp++ = VCP_SETREG (VCR_ADDR, VCP_TOVCPADDR (addr));
		addr += BASEWIDTH;
		y += yincr;
	}

	// End of frame (wait forever).
	*vcp = VCP_WAITY (32767);

	// Palette.
	unsigned *palette = (unsigned *)s_palette;
	*palette++ = VCP_SETPAL (0, 256);
	palette += 256;
	*palette = VCP_RTS;

	// Configure the main layer 1 VCP to call our VCP.
	*((unsigned *)0x40000010) = VCP_JMP (s_vcp);

	// The layer 2 VCP should do nothing.
	*((unsigned *)0x40000020) = VCP_WAITY (32767);
}

void VID_SetPalette (unsigned char *palette)
{
	unsigned *dst = &((unsigned *)s_palette)[1];
	const unsigned a = 255;
	for (int i = 0; i < 256; ++i)
	{
		unsigned r = (unsigned)palette[i * 3];
		unsigned g = (unsigned)palette[i * 3 + 1];
		unsigned b = (unsigned)palette[i * 3 + 2];
#ifdef __MRISC32_PACKED_OPS__
		dst[i] = _mr32_pack_h (_mr32_pack (a, g), _mr32_pack (b, r));
#else
		dst[i] = (a << 24) | (b << 16) | (g << 8) | r;
#endif
	}
}

void VID_ShiftPalette (unsigned char *palette)
{
	VID_SetPalette (palette);
}

void VID_Init (unsigned char *palette)
{
	MC1_AllocInit ();

	// Video buffers that need to be in VRAM.
	const size_t vcp_size = (5 + BASEHEIGHT * 2) * sizeof (unsigned);
	const size_t palette_size = (2 + 256) * sizeof (unsigned);
	const size_t framebuffer_size = BASEWIDTH * BASEHEIGHT * sizeof (byte);
	s_vcp = MC1_VRAM_Alloc (vcp_size);
	s_palette = MC1_VRAM_Alloc (palette_size);
	s_framebuffer = MC1_VRAM_Alloc (framebuffer_size);
	if (s_framebuffer == NULL)
	{
		printf ("Error: Not enough VRAM!\n");
		exit (1);
	}

	// Allocate memory for the various buffers that Quake needs.
	const size_t vbuffer_size = BASEWIDTH * BASEHEIGHT * sizeof (byte);
	const size_t zbuffer_size = BASEWIDTH * BASEHEIGHT * sizeof (short);
	const size_t surfcache_size = 4 * 1024 * 1024 * sizeof (byte);
	s_vbuffer = MC1_Alloc (vbuffer_size);
	if (MC1_IsVRAMPtr (s_vbuffer))
		Con_Printf ("Using VRAM for the pixel buffer\n");
	s_zbuffer = MC1_Alloc (zbuffer_size);
	if (MC1_IsVRAMPtr (s_zbuffer))
		Con_Printf ("Using VRAM for the Z buffer\n");
	s_surfcache = MC1_Alloc (surfcache_size);
	if (MC1_IsVRAMPtr (s_surfcache))
		Con_Printf ("Using VRAM for the surface cache\n");

	printf (
		"VID_Init: Resolution = %d x %d\n"
		"          Framebuffer @ 0x%08x (%d)\n"
		"          Palette     @ 0x%08x (%d)\n",
		BASEWIDTH,
		BASEHEIGHT,
		(unsigned)s_framebuffer,
		(unsigned)s_framebuffer,
		(unsigned)(s_palette + 4),
		(unsigned)(s_palette + 4));

	// Create the VCP.
	VID_CreateVCP ();

	// Set up the vid structure that is used by the Quake rendering engine.
	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = s_vbuffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;

	d_pzbuffer = (short *)s_zbuffer;
	D_InitCaches (s_surfcache, surfcache_size);
}

void VID_Shutdown (void)
{
	MC1_Free (s_surfcache);
	MC1_Free (s_zbuffer);
	MC1_Free (s_vbuffer);
}

void VID_Update (vrect_t *rects)
{
	vrect_t *rect;
	for (rect = rects; rect != NULL; rect = rect->pnext)
	{
		size_t bytes_per_row = rect->width;
		unsigned num_rows = rect->height;
		const byte *src = vid.buffer + rect->y * BASEWIDTH + rect->x;
		unsigned i;

		byte *dst = s_framebuffer + rect->y * BASEWIDTH + rect->x;
		if (bytes_per_row == BASEWIDTH)
		{
			bytes_per_row *= num_rows;
			num_rows = 1;
		}

		for (i = 0; i < num_rows; ++i)
		{
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
			memcpy (dst, src, bytes_per_row);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

			src += BASEWIDTH;
			dst += BASEWIDTH;
		}
	}
}

/*
================
D_BeginDirectRect
================
*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}

/*
================
D_EndDirectRect
================
*/
void D_EndDirectRect (int x, int y, int width, int height)
{
}
