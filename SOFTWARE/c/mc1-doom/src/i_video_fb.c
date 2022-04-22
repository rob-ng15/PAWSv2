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
   screen_mode( 0, MODE_RGB, 0 );
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

unsigned char PAWSKEYlookup[] = {
    0x00, KEY_F9, 0x00, 0x003, KEY_F3, KEY_F1, KEY_F2, KEY_F12, 0x00, KEY_F10, KEY_F8, KEY_F6, KEY_F4, KEY_TAB, 0x00, 0x00,     // 0x00 - 0x0f
    0x00, KEY_RALT, 0x00, 0x00, 'q', 0x00, '1', 0x00, 0x00, 0x00, 'z', 's', 'a', 'w', '2', 0x00,                                // 0x10 - 0x1f
    0x00, 'c', 'x', 'd', 'e', '4', '3', 0x00, 0x00, ' ', 'v', 'f', 't', 'r', '5', 0x00,                                         // 0x20 - 0x2f
    0x00, 'n', 'b', 'h', 'g', 'y', '6', 0x00, 0x00, 0x00, 'm', 'j', 'u', '7', '8', 0x00,                                        // 0x30 - 0x3f
    0x00, 0x00, 'k', 'i', 0x00, '0', '9', 0x00, 0x00, 0x00, 0x00, 'l', 0x00, 'p', KEY_MINUS, 0x00,                              // 0x40 - 0x4f
    0x00, 0x00, 0x00, 0x00, 'o', KEY_EQUALS, 0x00, 0x00, 0x00, KEY_RSHIFT, KEY_ENTER, 0x00, 0x00, 0x00, 0x00, 0x00,             // 0x50 - 0x5f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, KEY_BACKSPACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // 0x60 - 0x6f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, KEY_ESCAPE, 0x00, KEY_F11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // 0x70 - 0x7f
    0x00, 0x00, 0x00, KEY_F7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                           // 0x80 - 0x8f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0x90 - 0x9f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xa0 - 0xaf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xb0 - 0xbf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xc0 - 0xcf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xd0 - 0xdf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xe0 - 0xef
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00                              // 0xf0 - 0xff
};

unsigned char PAWSKEYtoDOOM( unsigned short keycode ) {
    if( keycode & 0x100 ) {
        switch( keycode ) {
            case 0x175: return KEY_UPARROW;
            case 0x172: return KEY_DOWNARROW;
            case 0x174: return KEY_RIGHTARROW;
            case 0x16b: return KEY_LEFTARROW;
            case 0x114: return KEY_RCTRL;
            case 0x111: return KEY_RALT;
            default:    return 0;
        }
    } else {
        return PAWSKEYlookup[ keycode ];
    }
}

void I_StartTic (void) {
    event_t event;
    unsigned short keycode, doomkeycode;

    if( ps2_event_available() ) {
        keycode = ps2_event_get();
        doomkeycode = PAWSKEYtoDOOM( keycode & 0x1ff );
        if( doomkeycode ) {
            event.data1 = doomkeycode;
            event.type = keycode & 0x200 ? ev_keydown : ev_keyup; D_PostEvent( &event );
        }
    }
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
