// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
//      DOOM graphics renderer for LiteX framebuffer
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

static uint32_t s_palette[256];

static inline uint32_t color_to_argb8888 (
   unsigned int r,
   unsigned int g,
   unsigned int b
) {
   return 0xff000000u | (b << 16) | (g << 8) | r;
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
    gpu_printf_centre( GREY80, 160, 0, 0, 0, 0, "STARTING DRAWING FRAME %0d", __pb_count++ );
    gpu_printf_centre( GREY80, 160, 231, 0, 0, 0, "(%0d x %0d) to (%0d x %0d)", SCREENWIDTH,SCREENHEIGHT,FB_WIDTH,FB_HEIGHT );
    gpu_pixelblock_start( 0, 20, FB_WIDTH );
    for (int i = 0; i < SCREENHEIGHT*SCREENWIDTH; i++) {
        int rgb = s_palette[ *src++ ];
        gpu_pixelblock_pixel24( ( rgb & 0xff0000 ) >> 16, ( rgb & 0xff00 ) >> 8, rgb & 0xff );
    }
    gpu_pixelblock_stop();

    //fb_swap_buffers();
}

void I_ReadScreen (byte* scr) {
    memcpy (scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}

void I_SetPalette (byte* palette) {
    for (int i = 0; i < 256; ++i) {
        unsigned int r = (unsigned int)gammatable[usegamma][*palette++];
        unsigned int g = (unsigned int)gammatable[usegamma][*palette++];
        unsigned int b = (unsigned int)gammatable[usegamma][*palette++];
        s_palette[i] = color_to_argb8888 (r, g, b);
    }
}
