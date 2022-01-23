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

#include "../library/PAWSlibrary.h"

#define FB_WIDTH 320
#define FB_HEIGHT = 240;

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

   gpu_cs();
}

void I_ShutdownGraphics (void) {
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

void I_FinishUpdate (void) {
    unsigned char *src = (unsigned char*)screens[0];
    gpu_pixelblock_start( 0, 0, FB_WIDTH );
    for (int i = 0; i < SCREENHEIGHT*SCREENWIDTH; ++i) gpu_pixelblock_pixel7( *src++ );
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
