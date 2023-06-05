// dear imgui: "null" example application
// (compile and link imgui, create context, run headless with NO INPUTS, NO GRAPHICS OUTPUT)
// This is useful to test building, but you cannot interact with anything here!
#include "imgui.h"
#include "imgui_sw.h"

#include <stdio.h>
#include <cstdlib>

extern "C" {
#include <PAWSlibrary.h>
}

uint32_t *fb_base;

unsigned char mouse_sprite[] = {
#include "mouse_sprite.h"
};

void* operator new(size_t size) {
   return ImGui::MemAlloc(size);
}

extern unsigned int volatile *DMASOURCE;
extern unsigned int volatile *DMADEST;
extern unsigned int volatile *DMACOUNT;
extern unsigned char volatile *DMAMODE;
extern unsigned char volatile *DMASET;

void cpp_DMASTART( void *source, void *destination, unsigned int count, unsigned char mode ) {
    *DMASOURCE = (unsigned long)source; *DMADEST = (unsigned long)destination; *DMACOUNT = count; *DMAMODE = mode;
}
void *cpp_paws_memcpy( void * destination, const void *source, size_t count ) {
    cpp_DMASTART( (void *)source, (void *)destination, count, 3 );
    return( destination );
}
void *cpp_paws_memset( void *destination, int value, size_t count ) {
    *DMASET = (unsigned char)value; cpp_DMASTART( (void *)DMASET, destination, count, 4 );
    return( destination );
}

unsigned int __modifiers = 0;
static ImGuiKey ImGui_ImplPAWS_KeycodeToImGuiKey(short keycode)
{
    switch (keycode & 0x1ff)
    {
        case 0x0d: return ImGuiKey_Tab;
        case 0x16b: return ImGuiKey_LeftArrow;
        case 0x174: return ImGuiKey_RightArrow;
        case 0x175: return ImGuiKey_UpArrow;
        case 0x172: return ImGuiKey_DownArrow;

        case 0x17d: return ImGuiKey_PageUp;
        case 0x17a: return ImGuiKey_PageDown;
        case 0x16c: return ImGuiKey_Home;
        case 0x169: return ImGuiKey_End;
        case 0x170: return ImGuiKey_Insert;
        case 0x171: return ImGuiKey_Delete;

        case 0x66: return ImGuiKey_Backspace;
        case 0x29: return ImGuiKey_Space;
        case 0x5a: return ImGuiKey_Enter;
        case 0x76: return ImGuiKey_Escape;
        //case 0x52: return ImGuiKey_Apostrophe;
        //case 0x41: return ImGuiKey_Comma;
        //case 0x4e: return ImGuiKey_Minus;
        //case 0x49: return ImGuiKey_Period;
        //case 0x4a: return ImGuiKey_Slash;
        //case 0x4c: return ImGuiKey_Semicolon;
        //case 0x55: return ImGuiKey_Equal;
        //case 0x54: return ImGuiKey_LeftBracket;
        //case 0x61: return ImGuiKey_Backslash;
        //case 0x5b: return ImGuiKey_RightBracket;
        //case 0x0e: return ImGuiKey_GraveAccent;
        //case 0x58: return ImGuiKey_CapsLock;
        //case SDLK_SCROLLLOCK: return ImGuiKey_ScrollLock;
        //case 0x77: return ImGuiKey_NumLock;
        //case SDLK_PRINTSCREEN: return ImGuiKey_PrintScreen;
        //case SDLK_PAUSE: return ImGuiKey_Pause;

        //case 0x70: return ImGuiKey_Keypad0;
        //case 0x69: return ImGuiKey_Keypad1;
        //case 0x72: return ImGuiKey_Keypad2;
        //case 0x7a: return ImGuiKey_Keypad3;
        //case 0x6b: return ImGuiKey_Keypad4;
        //case 0x73: return ImGuiKey_Keypad5;
        //case 0x74: return ImGuiKey_Keypad6;
        //case 0x6c: return ImGuiKey_Keypad7;
        //case 0x75: return ImGuiKey_Keypad8;
        //case 0x7d: return ImGuiKey_Keypad9;
        //case 0x71: return ImGuiKey_KeypadDecimal;
        //case 0x14a: return ImGuiKey_KeypadDivide;
        //case 0x7c: return ImGuiKey_KeypadMultiply;
        //case 0x7b: return ImGuiKey_KeypadSubtract;
        //case 0x79: return ImGuiKey_KeypadAdd;
        case 0x15a: return ImGuiKey_KeyPadEnter;
        //case SDLK_KP_EQUALS: return ImGuiKey_KeypadEqual;

        case 0x14:  case 0x114: if( keycode & 0x200 ) { __modifiers |= ImGuiKeyModFlags_Ctrl; } else { __modifiers &= !ImGuiKeyModFlags_Ctrl; } return -1;
        case 0x12:  case 0x59:  if( keycode & 0x200 ) { __modifiers |= ImGuiKeyModFlags_Shift; } else { __modifiers &= !ImGuiKeyModFlags_Shift; } return -1;
        case 0x11:  case 0x111: if( keycode & 0x200 ) { __modifiers |= ImGuiKeyModFlags_Alt; } else { __modifiers &= !ImGuiKeyModFlags_Alt; } return -1;
        case 0x11f: case 0x127: if( keycode & 0x200 ) { __modifiers |= ImGuiKeyModFlags_Super; } else { __modifiers &= !ImGuiKeyModFlags_Super; } return -1;
        //case 0x59: return ImGuiKey_RightShift;
        //case 0x111: return ImGuiKey_RightAlt;
        //case 0x127: return ImGuiKey_RightSuper;
        //case 0x12f: return ImGuiKey_Menu;

        //case 0x45: return ImGuiKey_0;
        //case 0x16: return ImGuiKey_1;
        //case 0x1e: return ImGuiKey_2;
        //case 0x26: return ImGuiKey_3;
        //case 0x25: return ImGuiKey_4;
        //case 0x2e: return ImGuiKey_5;
        //case 0x36: return ImGuiKey_6;
        //case 0x3d: return ImGuiKey_7;
        //case 0x3e: return ImGuiKey_8;
        //case 0x46: return ImGuiKey_9;
        case 0x1c: return ImGuiKey_A;
        //case 0x32: return ImGuiKey_B;
        case 0x21: return ImGuiKey_C;
        //case 0x23: return ImGuiKey_D;
        //case 0x24: return ImGuiKey_E;
        //case 0x2b: return ImGuiKey_F;
        //case 0x34: return ImGuiKey_G;
        //case 0x33: return ImGuiKey_H;
        //case 0x43: return ImGuiKey_I;
        //case 0x3b: return ImGuiKey_J;
        //case 0x42: return ImGuiKey_K;
        //case 0x4b: return ImGuiKey_L;
        //case 0x3a: return ImGuiKey_M;
        //case 0x31: return ImGuiKey_N;
        //case 0x44: return ImGuiKey_O;
        //case 0x4d: return ImGuiKey_P;
        //case 0x15: return ImGuiKey_Q;
        //case 0x2d: return ImGuiKey_R;
        //case 0x1b: return ImGuiKey_S;
        //case 0x2c: return ImGuiKey_T;
        //case 0x3c: return ImGuiKey_U;
        case 0x2a: return ImGuiKey_V;
        //case 0x1d: return ImGuiKey_W;
        case 0x22: return ImGuiKey_X;
        case 0x35: return ImGuiKey_Y;
        case 0x1a: return ImGuiKey_Z;

        //case 0x05: return ImGuiKey_F1;
        //case 0x06: return ImGuiKey_F2;
        //case 0x04: return ImGuiKey_F3;
        //case 0x0c: return ImGuiKey_F4;
        //case 0x03: return ImGuiKey_F5;
        //case 0x0b: return ImGuiKey_F6;
        //case 0x83: return ImGuiKey_F7;
        //case 0x0a: return ImGuiKey_F8;
        //case 0x01: return ImGuiKey_F9;
        //case 0x09: return ImGuiKey_F10;
        //case 0x78: return ImGuiKey_F11;
        //case 0x07: return ImGuiKey_F12;
    }
    return -1;
}

int main(int, char**)
{
    short mouse_x, mouse_y, mouse_btns;
    set_sprite_bitmaps( UPPER_LAYER, 15, mouse_sprite );

    fb_base = (uint32_t*)malloc( 320 * 240 * 4); cpp_paws_memset( fb_base, 0, (uint32_t)320*240*4 );
    bitmap_256( TRUE ); bitmap_display( 1 ); bitmap_draw( 1 ); gpu_cs(); gpu_pixelblock_mode( PB_WRITEALL );
    ps2_keyboardmode( TRUE );

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    imgui_sw::bind_imgui_painting();
    imgui_sw::make_style_fast();

    int n = 0;
    for (;;) {
        ++n;
        io.DisplaySize = ImVec2(320, 240);
        io.DeltaTime = 1.0f / 60.0f;

        // PROCESS KEYBOARD INPUTS
        if( ps2_event_available ) {
            short keycode = ps2_event_get(); ImGuiKey thiskey = ImGui_ImplPAWS_KeycodeToImGuiKey( keycode );
            if( thiskey >= 0 ) {
                io.KeyMap[thiskey] = io.KeysDown[thiskey] = ( keycode & 0x200 ) >> 9;
            } else {
                io.KeyMods = __modifiers;
                io.KeyCtrl = ( __modifiers & ImGuiKeyModFlags_Ctrl );
                io.KeyShift = ( __modifiers & ImGuiKeyModFlags_Shift ) >> 1;
                io.KeyAlt = ( __modifiers & ImGuiKeyModFlags_Alt ) >> 2;
                io.KeySuper = ( __modifiers & ImGuiKeyModFlags_Super ) >> 3;
            }
        }

        // SHOW MOUSE CURSOR
        int mouse_cursor = ImGui::GetMouseCursor();
        get_mouse( &mouse_x, &mouse_y, &mouse_btns );
        io.MousePos = ImVec2( ( mouse_x >> 1 ), ( mouse_y >> 1 ) );             // set the mouse position
        io.MouseDown[ImGuiMouseButton_Left] = ( mouse_btns & 2 ) >> 1;          // set the mouse button states
        io.MouseDown[ImGuiMouseButton_Right] = ( mouse_btns & 4 ) >> 2;
        if( mouse_cursor != 0 ) { mouse_x -= 16; mouse_y -= 16; }               // adjust if focus point is not top left
        set_sprite( UPPER_LAYER, 15, ( mouse_cursor != ImGuiMouseCursor_None ), mouse_x, mouse_y, mouse_cursor, SPRITE_DOUBLE );

        ImGui::NewFrame();

        ImGui::ShowDemoWindow(NULL);

        ImGui::SetNextWindowSize(ImVec2(150, 100));
        ImGui::Begin("Test");
        ImGui::Text("Hello, world!");
        ImGui::Text("Frame: %d",n);
        ImGui::End();

        ImGui::Render();
        imgui_sw::paint_imgui((uint32_t*)fb_base,320,240);
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
        gpu_pixelblockARGB( 0, 0, 320, 240, fb_base );
#else
        gpu_pixelblockABGR( 0, 0, 320, 240, fb_base );
#endif
        cpp_paws_memset( fb_base, 0, 320*240*4 );

        if( mouse_btns == 3 ) break;
    }

    while( get_buttons() != 1 );
    ImGui::DestroyContext();
    return 0;
}
