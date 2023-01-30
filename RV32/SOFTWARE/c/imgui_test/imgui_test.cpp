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

unsigned int *fb_base;

extern int volatile *DMASOURCEADD;
extern int volatile *DMADESTADD4;
extern unsigned char volatile *DMACYCLES;
extern unsigned int volatile *DMASOURCE;
extern unsigned int volatile *DMADEST;
extern unsigned int volatile *DMACOUNT;
extern unsigned char volatile *DMAMODE;
extern unsigned char volatile *DMASET;
extern unsigned int volatile *DMASETRGB;

void* operator new(size_t size) {
   return ImGui::MemAlloc(size);
}

void cpp_DMASTART( void *source, void *destination, unsigned int count, unsigned char mode ) {
    *DMASOURCE = (unsigned int)source; *DMADEST = (unsigned int)destination; *DMACOUNT = count; *DMAMODE = mode;
}
void *cpp_paws_memcpy( void * destination, const void *source, size_t count ) {
    cpp_DMASTART( (void *)source, (void *)destination, count, 3 );
    return( destination );
}
void *cpp_paws_memset( void *destination, int value, size_t count ) {
    *DMASET = (unsigned char)value; cpp_DMASTART( (void *)DMASET, destination, count, 4 );
    return( destination );
}

int main(int, char**)
{
    fb_base = (unsigned int *)malloc( 320 * 240 * 4); cpp_paws_memset( fb_base, 0, 320*240*4 );
    screen_mode( 0, MODE_GREY, 0 ); bitmap_256( TRUE ); bitmap_display( 1 ); bitmap_draw( 1 ); gpu_cs(); gpu_pixelblock_mode( PB_REMAP | PB_WRITEALL );

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    imgui_sw::bind_imgui_painting();
    imgui_sw::make_style_fast();

    int n = 0;
    for (;;) {
        ++n;
        io.DisplaySize = ImVec2(320, 240);
        io.DeltaTime = 1.0f / 60.0f;
        ImGui::NewFrame();

        ImGui::ShowDemoWindow(NULL);

        ImGui::SetNextWindowSize(ImVec2(150, 100));
        ImGui::Begin("Test");
        ImGui::Text("Hello, world!");
        ImGui::Text("Frame: %d",n);
        ImGui::End();

        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::Render();
        imgui_sw::paint_imgui((uint32_t*)fb_base,320,240);

        gpu_pixelblockARGB( 0, 0, 320, 240, fb_base );
        cpp_paws_memset( fb_base, 0, 320*240*4 );

        if( get_buttons() != 1 ) break;
    }

    printf("DestroyContext()\n");
    ImGui::DestroyContext();
    return 0;
}
