// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
//      DOOM graphics renderer for PAWS framebuffer
//
//-----------------------------------------------------------------------------

#include <stdlib.h>

#include "d_event.h"
#include "doomdef.h"
#include "doomstat.h"
#include "d_main.h"
#include "i_system.h"
#include "m_argv.h"
#include "v_video.h"

#include <PAWSlibrary.h>

#define FB_WIDTH 320
#define FB_HEIGHT 240

unsigned char PAWSKEYlookup[] = {
    0x00, KEY_F9, 0x00, KEY_F5, KEY_F3, KEY_F1, KEY_F2, KEY_F12, 0x00, KEY_F10, KEY_F8, KEY_F6, KEY_F4, KEY_TAB, 0x00, 0x00,     // 0x00 - 0x0f
    0x00, KEY_LALT, 0x00, 0x00, 'q', 0x00, '1', 0x00, 0x00, 0x00, 'z', 's', 'a', 'w', '2', 0x00,                                // 0x10 - 0x1f
    0x00, 'c', 'x', 'd', 'e', '4', '3', 0x00, 0x00, ' ', 'v', 'f', 't', 'r', '5', 0x00,                                         // 0x20 - 0x2f
    0x00, 'n', 'b', 'h', 'g', 'y', '6', 0x00, 0x00, 0x00, 'm', 'j', 'u', '7', '8', 0x00,                                        // 0x30 - 0x3f
    0x00, 0x00, 'k', 'i', 'o', '0', '9', 0x00, 0x00, 0x00, 0x00, 'l', 0x00, 'p', KEY_MINUS, 0x00,                              // 0x40 - 0x4f
    0x00, 0x00, 0x00, 0x00, 0x00, KEY_EQUALS, 0x00, 0x00, 0x00, KEY_RSHIFT, KEY_ENTER, 0x00, 0x00, 0x00, 0x00, 0x00,             // 0x50 - 0x5f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, KEY_BACKSPACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // 0x60 - 0x6f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, KEY_ESCAPE, KEY_PAUSE, KEY_F11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    // 0x70 - 0x7f
    0x00, 0x00, 0x00, KEY_F7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                           // 0x80 - 0x8f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0x90 - 0x9f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xa0 - 0xaf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xb0 - 0xbf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xc0 - 0xcf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xd0 - 0xdf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xe0 - 0xef
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00                              // 0xf0 - 0xff
};

void I_InitGraphics (void) {
    // Only initialize once.
    static int initialized = 0;
    if (initialized)
        return;

    initialized = 1;
    screens[0] = (byte*)0x2020000; screen_order( LAYER_CHARACTERMAP, LAYER_BITMAP_0, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE );
    screen_mode( MODE_RGBM ); bitmap_256( TRUE ); use_palette( TRUE );
}

void I_ShutdownGraphics (void) {
}

void I_WaitVBL (int count) {
    unsigned int framecount = get_framecount() + count;

    if( count > 1 )
        while( get_framecount() < framecount );
}

void I_StartFrame (void) {
   // er?
}

unsigned char PAWSKEYtoDOOM( unsigned short keycode ) {
    switch( keycode ) {
        case 0x175: return KEY_UPARROW;
        case 0x172: return KEY_DOWNARROW;
        case 0x174: return KEY_RIGHTARROW;
        case 0x16b: return KEY_LEFTARROW;
        case 0x114: return KEY_RCTRL;
        case 0x111: return KEY_RALT;
        default:    return keycode & 0x100 ? 0 : PAWSKEYlookup[ keycode & 0xff ];
    }
}

void I_StartTic (void) {
    event_t event; unsigned short keycode, doomkeycode;
    static short joystick_buttons, old_joystick_buttons;

    if( ps2_event_available() ) {
        keycode = ps2_event_get();
        doomkeycode = PAWSKEYtoDOOM( keycode & 0x1ff );
        if( doomkeycode ) {
            event.type = ( keycode & 0x200 ) ? ev_keydown : ev_keyup;
            event.data1 = doomkeycode;
            D_PostEvent( &event );
        }
    }

    joystick_buttons = get_buttons() >> 1;
    if( ( joystick_buttons & 0x3 ) || ( joystick_buttons != old_joystick_buttons ) ) {
        event.type = ev_joystick;
        event.data1 = joystick_buttons & 0x3;
        event.data2 = ( _rv64_bext( joystick_buttons, 4 ) ? -2 : 0 ) +
                      ( _rv64_bext( joystick_buttons, 5 ) ? 2 : 0 );
        event.data3 = ( _rv64_bext( joystick_buttons, 2 ) ? -2 : 0 ) +
                      ( _rv64_bext( joystick_buttons, 3 ) ? 2 : 0 );
        D_PostEvent( &event );
    }
    old_joystick_buttons = joystick_buttons;
}

void I_UpdateNoBlit (void) {
    // what is this?
}

void I_FinishUpdate (void) {
    memcpy( (byte*)0x2001900, screens[0], SCREENWIDTH * SCREENHEIGHT );
}

void I_ReadScreen (byte* scr) {
    memcpy( scr, screens[0], SCREENWIDTH * SCREENHEIGHT );
}

// SET THE NEW PALETTE
void I_SetPalette (byte* palette) {
    for (int i = 0; i < 256; i++) {
        set_palette( i, PACKRGB( gammatable[usegamma][*palette], gammatable[usegamma][*(palette+1)], gammatable[usegamma][*(palette+2)] ) );
        palette += 3;
    }
}
