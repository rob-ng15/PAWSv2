#include "wl_def.h"

byte *gfxbuf = (byte *)0x2020000;
byte *graphmem = (byte *)0x2000000;

void keyboard_handler(int code, int press);
void keyboard_handlerx(int code, int press);

void DisplayTextSplash(byte *text);

/*
==========================
=
= Quit
=
==========================
*/

void Quit(char *error)
{
	memptr screen = NULL;

	if (!error || !*error) {
		CA_CacheGrChunk(ORDERSCREEN);
		screen = grsegs[ORDERSCREEN];
		WriteConfig();
	} else if (error) {
		CA_CacheGrChunk(ERRORSCREEN);
		screen = grsegs[ERRORSCREEN];
	}
	
	ShutdownId();
	
	if (screen) {
		/* doesn't look too good on console at the moment ...*/
		/* DisplayTextSplash(screen); */
	}
	
	if (error && *error) {
		fprintf(stderr, "Quit: %s\n", error);
		exit(EXIT_FAILURE);
 	}
	exit(EXIT_SUCCESS);
}

void VL_WaitVBL(int vbls)
{
    unsigned int framecount = get_framecount() + vbls;

    while( get_framecount() < framecount );
	fprintf(stderr,"VL_WaitVBL\n");
}

void VW_UpdateScreen()
{
	VL_WaitVBL(1); 
	memcpy(graphmem, gfxbuf, vwidth*vheight);
	fprintf(stderr,"VW_UpdateScreen\n");
}

/*
=======================
=
= VL_Startup
=
=======================
*/

void VL_Startup()
{
	printf("GFX gfxbuf=%x graphmem=%x\n",gfxbuf,graphmem);
	bitmap_display( 1 ); screen_mode( 0, MODE_RGBM, 0 ); bitmap_256( TRUE ); //use_palette( TRUE );
}

/*
=======================
=
= VL_Shutdown
=
=======================
*/

void VL_Shutdown()
{
//	keyboard_close();
}

/* ======================================================================== */

/*
=================
=
= VL_SetPalette
=
=================
*/

void VL_SetPalette(const byte *palette)
{
	VL_WaitVBL(1);
	
    for (int i = 0; i < 256; i++) {
        set_palette( i, PACKRGB( *palette, *(palette+1), *(palette+2) ) );
        palette += 3;
    }

    fprintf(stderr,"VL_SetPalette\n");
}

/*
=================
=
= VL_GetPalette
=
=================
*/

void VL_GetPalette(byte *palette)
{
	int i, r, g, b;
	
	for (i = 0; i < 256; i++) {
//		vga_getpalette(i, &r, &g, &b);
		palette[i*3+0] = r;
		palette[i*3+1] = g;
		palette[i*3+2] = b;
	}
}

/*
=============================================================================

					GLOBAL VARIABLES

=============================================================================
*/

static int SVGALibToScancode(int key)
{
	// switch(key) {
	// 	case SCANCODE_BREAK:
	// 	case SCANCODE_BREAK_ALTERNATIVE:
	// 		return 0xe1; /* paused */
	// 	case SCANCODE_CURSORBLOCKUP:
	// 		return sc_UpArrow;
	// 	case SCANCODE_CURSORBLOCKDOWN:
	// 		return sc_DownArrow;
	// 	case SCANCODE_CURSORBLOCKLEFT:
	// 		return sc_LeftArrow;
	// 	case SCANCODE_CURSORBLOCKRIGHT:
	// 		return sc_RightArrow;
	// 	case SCANCODE_HOME:
	// 		return sc_Home;
	// 	case SCANCODE_END:
	// 		return sc_End;
	// 	case SCANCODE_PAGEUP:
	// 		return sc_PgUp;
	// 	case SCANCODE_PAGEDOWN:
	// 		return sc_PgDn;
	// 	case SCANCODE_INSERT:
	// 		return sc_Insert;
	// 	case SCANCODE_REMOVE:
	// 		return sc_Delete;
	// 	default: /* rest should be the same hopefully */
	// 		return key;
	// }
}

void keyboard_handlerx(int code, int press)
{
//	keyboard_handler(SVGALibToScancode(code), press);
}

void INL_Update()
{
//	while (keyboard_update()) ; /* get all events */
}

int main(int argc, char *argv[])
{
	vwidth = 320;
	vheight = 200;

	return WolfMain(argc, argv);
}
