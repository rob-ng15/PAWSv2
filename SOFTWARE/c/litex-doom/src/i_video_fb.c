// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
//      DOOM graphics renderer for PAWS framebuffer using pixelblock
//
//-----------------------------------------------------------------------------

#include <stdlib.h>

#include "doomdef.h"
#include "doomstat.h"
#include "d_main.h"
#include "i_system.h"
#include "m_argv.h"
#include "v_video.h"

#include <PAWSlibrary.h>

#define FB_WIDTH 320
#define FB_HEIGHT 200

static uint8_t s_palette[256];

static inline uint32_t color_to_argb8888 (
   unsigned int r,
   unsigned int g,
   unsigned int b
) {
   return 0xff000000u | (b << 16) | (g << 8) | r;
}

static inline uint8_t color_to_argbpaws ( unsigned char r, unsigned char g, unsigned char b ) {
    uint8_t colour = 0;

    if( g & 128 ) colour = 64;
    colour += ( ( r & 0xc0 ) >> 2 );
    colour += ( ( g & 0x60 ) >> 3 );
    colour += ( ( b & 0xc0 ) >> 6 );

    fprintf(stderr,"( %0d, %0d, %0d ) -> 0x%0x ",r,g,b,( colour == 64 ) ? 68 : colour );
    return( ( colour == 64 ) ? 68 : colour );
}

void I_InitGraphics (void) {
   // Only initialize once.
   static int initialized = 0;
   if (initialized)
     return;
   initialized = 1;

   screens[0] = (byte*)malloc (SCREENWIDTH * SCREENHEIGHT);
   screen_mode( 0, MODE_RGB );
   gpu_cs();
}

void I_ShutdownGraphics (void) {
    free (screens[0]);
}

void I_WaitVBL (int count) {

}

void I_StartFrame (void) {
   // er?
}

void I_StartTic (void) {
}

void I_UpdateNoBlit (void) {
    // what is this?
}

int __pb_count = 0;
void I_FinishUpdate (void) {
    unsigned char *src = (unsigned char*)screens[0];
    gpu_rectangle( TRANSPARENT, 0, 0, 320, 8 );
    gpu_rectangle( TRANSPARENT, 0, 231, 320, 239 );
    gpu_printf_centre( 0x7f, 160, 0, 0, 0, 0, "STARTING DRAWING FRAME %0d", __pb_count++ );
    gpu_printf_centre( 0x7f, 160, 231, 0, 0, 0, "(%0d x %0d) to (%0d x %0d)", SCREENWIDTH,SCREENHEIGHT,FB_WIDTH,FB_HEIGHT );

    gpu_pixelblock_start( 0, 20, FB_WIDTH );
    for (int i = 0; i < SCREENHEIGHT*SCREENWIDTH; i++ ) {
        gpu_pixelblock_pixel7( s_palette[*src++] );
    }
    gpu_pixelblock_stop();
}

void I_ReadScreen (byte* scr) {
    memcpy (scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}

void I_SetPalette (byte* palette) {
    gpu_cs();
    gpu_printf_centre( 0x7f, 160, 0, 0, 0, 0, "NEW PALETTE LOADED" );
    for (int i = 0; i < 256; ++i) {
        s_palette[i] = color_to_argbpaws ( *palette++, *palette++, *palette++ );
        fprintf(stderr,"palette[%0d]= %0d or 0x%0x\n",i,s_palette[i],s_palette[i]);
        gpu_rectangle( s_palette[i], 32 + ( i & 0xf ) * 16, 20 + ( ( i & 0xf0 ) >> 4 ) * 12, 47 + ( i & 0xf ) * 16, 31 + ( ( i & 0xf0 ) >> 4 ) * 12 );
    }

    sleep1khz( 2000, 0 );
}
