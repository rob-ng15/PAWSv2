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

unsigned char PAWSKEYlookup[] = {
    0x00, KEY_F9, 0x00, KEY_F5, KEY_F3, KEY_F1, KEY_F2, KEY_F12, 0x00, KEY_F10, KEY_F8, KEY_F6, KEY_F4, KEY_TAB, 0x00, 0x00,     // 0x00 - 0x0f
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

void I_InitGraphics (void) {
    unsigned char volatile *DMA_REGS_B = (unsigned char volatile *)DMA_REGS;
    unsigned char volatile *DISPLAY_REGS_B = (unsigned char volatile *)DISPLAY_REGS;
    unsigned char volatile *GPU_REGS_B = (unsigned char volatile *)GPU_REGS; short volatile *GPU_REGS_H = (short volatile *)GPU_REGS;

    // Only initialize once.
    static int initialized = 0;
    if (initialized)
        return;
    initialized = 1; screens[0] = (byte*)0x2020000;

    DISPLAY_REGS_B[0x00] = 0; DISPLAY_REGS_B[0x01] = MODE_RGBM; DISPLAY_REGS_B[0x02] = 0; DISPLAY_REGS_B[0x15] = TRUE;         // Setup video hardware, RGBM, 256 colours with palette
    GPU_REGS_B[0x7a] = PB_WRITEALL; GPU_REGS_B[0xf4] = TRUE;  GPU_REGS_B[0xf2] = 2; GPU_REGS_B[0xf0] = 1;                      // DRAW TO FB 1 , DISPLAY FB 0, BITMAP DISPLAY 256, PIXELBLOCK WRITE 256

    DMA_REGS_B[0x0e] = 0; DMA_REGS[0] = (long)&DMA_REGS_B[0x0e];                                                                // CLEAR THE FRAMEBUFFERS USING MEMSET
    DMA_REGS[1] = 0x2000000; DMA_REGS[2] = 320*240; DMA_REGS_B[0x0c] = 4;
    DMA_REGS[1] = 0x2020000; DMA_REGS_B[0x0c] = 4;
}

void I_ShutdownGraphics (void) {
}

void I_WaitVBL (int count) {

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
        default:    return keycode & 0x100 ? 0 : PAWSKEYlookup[ keycode ];
    }
}

void I_StartTic (void) {
    unsigned char volatile *IO_REGS_B = (unsigned char volatile *)IO_REGS; short volatile *IO_REGS_H = (short volatile *)IO_REGS;
    event_t event; unsigned short keycode, doomkeycode;

    if( IO_REGS_B[ 0x100 ] ) {
        keycode = IO_REGS_H[ 0x81 ];
        doomkeycode = PAWSKEYtoDOOM( keycode & 0x1ff );
        if( doomkeycode ) {
            event.data1 = doomkeycode;
            event.type = (keycode&256)>>8; D_PostEvent( &event );
        }
    }
}

void I_UpdateNoBlit (void) {
    // what is this?
}

void I_FinishUpdate (void) {
    unsigned char volatile *DMA_REGS_B = (unsigned char volatile *)DMA_REGS;
    DMA_REGS[0] = (long)screens[0]; DMA_REGS[1] = 0x2001900; DMA_REGS[2] = FB_WIDTH * FB_HEIGHT; DMA_REGS_B[0x0c] = 3;                            // COPY THE WORK SCREEN TO VISIBLE, CENTRE VERTICALLY
}

void I_ReadScreen (byte* scr) {
    unsigned char volatile *DMA_REGS_B = (unsigned char volatile *)DMA_REGS;
    DMA_REGS[0] = (long)screens[0]; DMA_REGS[1] = (long)scr; DMA_REGS[2] = SCREENWIDTH * SCREENHEIGHT; DMA_REGS_B[0x0c] = 3;                       // COPY THE WORK SCREEN TO VISIBLE, CENTRE VERTICALLY
}

// SET THE NEW PALETTE
void I_SetPalette (byte* palette) {
    unsigned char volatile *DISPLAY_REGS_B = (unsigned char volatile *)DISPLAY_REGS;
    for (int i = 0; i < 256; i++) {
        DISPLAY_REGS_B[0x14] = i; DISPLAY_REGS[0x04] = ( gammatable[usegamma][*palette++] << 16 ) + ( gammatable[usegamma][*palette++] << 8 ) + gammatable[usegamma][*palette++];
    }
}
