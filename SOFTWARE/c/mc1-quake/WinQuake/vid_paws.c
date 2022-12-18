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
// vid_null.c -- null video driver to aid porting efforts

#include "quakedef.h"
#include "d_local.h"

#define MALLOC_MEMORY ( 30 * 1024 * 1024 )
#include <PAWSlibrary.h>

extern viddef_t	vid;				// global video state

#define	BASEWIDTH	320
#define	BASEHEIGHT	240
#define SURFCACHESIZE 256*1024
byte	*vid_buffer = (byte*)0x2020000;
byte	*dis_buffer = (byte*)0x2000000;
short	*zbuffer;
byte	*surfcache;

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];

void	VID_SetPalette (unsigned char *palette)
{

}

void	VID_ShiftPalette (unsigned char *palette)
{
	for (int i = 0; i < 256; i++) {
		set_palette( i, ( palette[i * 3] << 16 ) + ( palette[i * 3 + 1] << 8 ) + palette[i * 3 + 2] );
	}
}

void	VID_Init (unsigned char *palette)
{
	zbuffer = malloc( BASEWIDTH*BASEHEIGHT*sizeof(short) );
	surfcache = malloc( SURFCACHESIZE );

	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;

	d_pzbuffer = zbuffer;
	D_InitCaches (surfcache, SURFCACHESIZE);

	screen_mode( 0, MODE_RGBM, 0 );																								// Set screen layer order, RGBM mode, hi-res character/tiles
	bitmap_256( TRUE ); bitmap_display( 1 ); bitmap_draw( 2 ); use_palette( TRUE );												// 256 colour display, palette mode

	memset( (void * restrict)0x2000000, 0, 320*240 ); memset( (void * restrict)0x2020000, 0, 320*240 );															// WIPE THE FRAMEBUFFERS
}

void	VID_Shutdown (void)
{
}

void	VID_Update (vrect_t *rects)
{
	vrect_t *rect;
	for (rect = rects; rect != NULL; rect = rect->pnext)
	{
		size_t bytes_per_row = rect->width;
		unsigned num_rows = rect->height;
		const byte *src = vid.buffer + rect->y * BASEWIDTH + rect->x;
		unsigned i;

		byte *dst = dis_buffer + rect->y * BASEWIDTH + rect->x;
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


