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

// STORAGE FOR DOOM PALETTE MAPPED TO PAWSv2 GRRGGBB PALETTE
static uint8_t s_palette[256];

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

unsigned char PAWSKEYtoDOOM( unsigned short keycode ) {
    switch( keycode ) {
        case 0x008: case 0x133: return KEY_BACKSPACE;
        case 0x009:             return KEY_TAB;
        case 0x00d:             return KEY_ENTER;
        case 0x01b:             return KEY_ESCAPE;
        case 0x020:             return ' ';
        case 0x02b: case 0x03d: return KEY_EQUALS;
        case 0x02d: case 0x5f:  return KEY_MINUS;
        case 0x101: case 0x111: return KEY_F1;
        case 0x102: case 0x112: return KEY_F2;
        case 0x103: case 0x113: return KEY_F3;
        case 0x104: case 0x114: return KEY_F4;
        case 0x105: case 0x115: return KEY_F5;
        case 0x106: case 0x116: return KEY_F6;
        case 0x107: case 0x117: return KEY_F7;
        case 0x108: case 0x118: return KEY_F8;
        case 0x109: case 0x119: return KEY_F9;
        case 0x10a: case 0x11a: return KEY_F10;
        case 0x10b: case 0x11b: return KEY_F11;
        case 0x10c: case 0x11c: return KEY_F12;
        case 0x141:             return KEY_UPARROW;
        case 0x142:             return KEY_DOWNARROW;
        case 0x143:             return KEY_RIGHTARROW;
        case 0x144:             return KEY_LEFTARROW;
        default:    return 0;
    }
}

void I_StartTic (void) {
    event_t event;
    //static unsigned short lastjoystick = 1;
    //unsigned short thisjoystick = get_buttons();

    if( ps2_character_available() ) {
        unsigned short doomcode = PAWSKEYtoDOOM( ps2_inputcharacter() );
        if( !doomcode ) {
            event.data1 = doomcode;
            event.type = ev_keydown; D_PostEvent( &event );
            event.type = ev_keyup; D_PostEvent( &event );
        }
    }
    //if( lastjoystick != thisjoystick ) {
    //}
}

void I_UpdateNoBlit (void) {
    // what is this?
}

int __pb_count = 0;
void I_FinishUpdate (void) {
    unsigned char *src = (unsigned char*)screens[0];
    gpu_pixelblock_start( 0, 20, FB_WIDTH );
    for (int i = 0; i < SCREENHEIGHT*SCREENWIDTH; i++ ) {
#ifndef PAWSv2PALETTE
        gpu_pixelblock_pixel7( s_palette[*src++] );
#else
        gpu_pixelblock_pixel7( paws_palette[*src++] );
#endif
    }
    gpu_pixelblock_stop();
}

void I_ReadScreen (byte* scr) {
    memcpy (scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}

void I_SetPalette (byte* palette) {
#ifndef PAWSv2PALETTE
    for (int i = 0; i < 256; ++i) {
        s_palette[i] = color_to_argbpaws ( gammatable[usegamma][*palette++], gammatable[usegamma][*palette++], gammatable[usegamma][*palette++] );
    }
#else
#endif
}
