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

static inline uint8_t color_to_paws( unsigned char r, unsigned char g, unsigned char b ) {
    uint8_t paws;

    paws = ( r & 0xc0 );
    paws += ( ( g & 0xe0 ) >> 2 );
    paws += ( ( b & 0xc0 ) >> 5 );
    paws += ( ( r & 0x20 ) && ( b & 0x20 ) ) ? 1 : 0;

    return( paws );
}

void I_InitGraphics (void) {
   // Only initialize once.
   static int initialized = 0;
   if (initialized)
     return;
   initialized = 1;

   unsigned char volatile *GPU_REGS_B = (unsigned char volatile *)GPU_REGS;

   screens[0] = (byte*)0x2020000;
   screen_mode( 0, MODE_RGBM, 0 ); gpu_pixelblock_mode( PB_REMAP | PB_WRITEALL );  bitmap_256( TRUE );
   gpu_rectangle( BLACK, 0, 0, 319, 239 );
}

void I_ShutdownGraphics (void) {
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
            event.type = _rv32_bext( keycode, 9 ); D_PostEvent( &event );
        }
    }
}

void I_UpdateNoBlit (void) {
    // what is this?
}

void I_FinishUpdate (void) {
    unsigned char volatile *GPU_REGS_B = (unsigned char volatile *)GPU_REGS; short volatile *GPU_REGS_H = (short volatile *)GPU_REGS;
    unsigned char volatile *DMA_REGS_B = (unsigned char volatile *)DMA_REGS;

    GPU_REGS_B[0xf2] = 0; GPU_REGS_B[0x7a] = PB_REMAP | PB_WRITEALL;                                                                        // SET WRITE TO DISPLAYED IMAGE + SETUP PIXEL BLOCK REMAP
    GPU_REGS[0] = 0x140000; GPU_REGS[1] = 0x400140; GPU_REGS_B[0x16] = 10;                                                                  // SETUP PIXEL BLOCK FOR 320 WIDE TRANSFER
    DMA_REGS[0] = (int)screens[0]; DMA_REGS[1] = 0xd670; DMA_REGS[2] = SCREENWIDTH*SCREENHEIGHT/2; DMA_REGS_B[0x0c] = 7;                    // TRANSFER THE IMAGE FROM SCREEN[0] USING DMA PIXELBLOCK MODE
    GPU_REGS_B[0x78] = 3; GPU_REGS_B[0xf2] = 1; GPU_REGS_B[0x7a] = PB_WRITEALL;                                                             // STOP THE PIXELBLOCK, SET WRITE TO HIDDEN IMAGE, STOP PIXEL BLOCK REMAP
}

void I_ReadScreen (byte* scr) {
    memcpy (scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}

// SET THE PIXELBLOCK COLOUR REMAPPER
void I_SetPalette (byte* palette) {
    for (int i = 0; i < 256; i++)
        GPU_REGS[0x1f] = _rv32_pack( i, color_to_paws ( gammatable[usegamma][*palette++], gammatable[usegamma][*palette++], gammatable[usegamma][*palette++] ) );
}
