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

// MAP DOOM PALETTE TO DEFAULT PAWSv2 PALETTE
static uint8_t paws_palette[256] = {
     0, 96, 0, 66, 63, 0, 0, 0, 0, 96, 96, 96, 96, 65, 65, 96,        // 0 - 15
     58, 58, 58, 58, 58, 58, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,        // 16 - 31
     76, 76, 75, 75, 75, 75, 75, 75, 75, 75, 16, 16, 16, 74, 74, 74,        // 32 - 47
     63, 63, 125, 125, 125, 58, 58, 58, 57, 57, 57, 57, 57, 57, 57, 36,     // 48 - 63,
     120, 120, 37, 120, 120, 37, 37, 37, 98, 97, 97, 97, 97, 97, 96, 96,    // 64 - 79
     63, 63, 71, 71, 71, 71, 71, 70, 70, 70, 42, 42, 69, 69, 69, 68,        // 80 - 95
     68, 68, 68, 67, 67, 67, 21, 21, 66, 66, 66, 65, 65, 65, 65, 65,        // 96 - 111
     29, 29, 86, 24, 25, 25, 25, 122, 122, 83, 82, 82, 81, 81, 80, 96,      // 112 - 127
     42, 42, 69, 69, 68, 68, 68, 68, 67, 67, 67, 21, 21, 66, 66, 66,        // 128 - 143,
     68, 68, 67, 98, 98, 97, 97, 97, 68, 67, 67, 67, 21, 66, 97, 97,        // 144 - 159,
     61, 103, 40, 101, 36, 120, 120, 75, 63, 125, 125, 58, 53, 53, 53, 48,  // 160 - 175,
     48, 48, 79, 79, 79, 78, 78, 32, 77, 76, 76, 76, 75, 16, 74, 74,        // 176 - 191
     63, 43, 43, 43, 23, 23, 95, 3, 3, 3, 95, 94, 93, 92, 91, 1,            // 192 - 207
     63, 63, 125, 58, 57, 57, 57, 52, 52, 52, 52, 36, 36, 36, 36, 36,       // 208 - 223
     63, 127, 62, 62, 61, 61, 60, 60, 36, 120, 77, 76, 97, 97, 96, 96,      // 224 - 239
     1, 90, 89, 89, 88, 88, 0, 0, 57, 61, 55, 51, 111, 109, 107, 37       // 240 - 255
};

static inline uint8_t color_to_argbpaws ( unsigned char r, unsigned char g, unsigned char b ) {
    uint8_t colour = 0;

    if( g & 128 ) colour = 64;
    colour += ( ( r & 0xc0 ) >> 2 );
    colour += ( ( g & 0x60 ) >> 3 );
    colour += ( ( b & 0xc0 ) >> 6 );

    return( ( colour == 64 ) ? 68 : colour );
}

void I_InitGraphics (void) {
   // Only initialize once.
   static int initialized = 0;
   if (initialized)
     return;
   initialized = 1;

   screens[0] = (byte*)malloc (SCREENWIDTH * SCREENHEIGHT);
#ifndef PAWSv2PALETTE
   screen_mode( 0, MODE_RGB );
#endif
   gpu_pixelblock_mode( TRUE );
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
    gpu_pixelblock( 0, 20, SCREENWIDTH, SCREENHEIGHT, TRANSPARENT, screens[0] );
}

void I_ReadScreen (byte* scr) {
    memcpy (scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}

// SET THE PIXELBLOCK COLOUR REMAPPER
void I_SetPalette (byte* palette) {
#ifndef PAWSv2PALETTE
    for (int i = 0; i < 256; i++) {
        gpu_pixelblock_remap( i, color_to_argbpaws ( gammatable[usegamma][*palette++], gammatable[usegamma][*palette++], gammatable[usegamma][*palette++] ) );
    }
#else
    for(int i = 0; i < 256; i++ )
        gpu_pixelblock_remap( i, paws_palette[i] );
#endif
}
