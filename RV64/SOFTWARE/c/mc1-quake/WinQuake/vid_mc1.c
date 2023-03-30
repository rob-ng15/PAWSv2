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

#include <string.h>

#include <PAWSlibrary.h>

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

static void VID_CreateVCP (void)
{
}

void VID_SetPalette (unsigned char *palette)
{
}

void VID_ShiftPalette (unsigned char *palette)
{
}

void VID_Init (unsigned char *palette)
{
	// Video buffers that need to be in VRAM.
	s_framebuffer = (byte*)0x2020000;

	// Allocate memory for the various buffers that Quake needs.
	const size_t vbuffer_size = BASEWIDTH * BASEHEIGHT * sizeof (byte);
	const size_t zbuffer_size = BASEWIDTH * BASEHEIGHT * sizeof (short);
	const size_t surfcache_size = 4 * 1024 * 1024 * sizeof (byte);
	s_vbuffer = malloc (vbuffer_size);
	s_zbuffer = malloc (zbuffer_size);
	s_surfcache = malloc (surfcache_size);

	fprintf (stderr,
		"VID_Init: Resolution = %d x %d\n"
		"          Framebuffer @ 0x%016lx (%d)\n",
		BASEWIDTH,
		BASEHEIGHT,
		(long)s_framebuffer,
		(long)s_framebuffer);

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
	free (s_surfcache);
	free (s_zbuffer);
	free (s_vbuffer);
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