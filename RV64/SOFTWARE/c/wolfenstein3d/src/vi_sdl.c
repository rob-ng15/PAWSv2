#include "wl_def.h"

#include "SDL.h"

byte *gfxbuf = NULL;
SDL_Surface *surface;


void keyboard_handler(int code, int press);

int main (int argc, char *argv[])
{
	return WolfMain(argc, argv);
}

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
		//DisplayTextSplash(screen);
	}
	
	if (error && *error) {
		fprintf(stderr, "Quit: %s\n", error);
		exit(EXIT_FAILURE);
 	}
	exit(EXIT_SUCCESS);
}

void VL_WaitVBL(int vbls)
{
	long last = get_TimeCount() + vbls;
	while (last > get_TimeCount()) ;
}

void VW_UpdateScreen()
{
	//VL_WaitVBL(1); 
	//memcpy(surface->pixels, gfxbuf, vwidth*vheight);
	//SDL_UpdateRect(surface, 0, 0, 0, 0);
	
	SDL_Flip(surface);
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
	vwidth = 320;
	vheight = 200;
	
	if (MS_CheckParm("x2")) {
		vwidth *= 2;
		vheight *= 2;
	} else if (MS_CheckParm("x3")) {
		vwidth *= 3;
		vheight *= 3;
	}
	
	//if (gfxbuf == NULL) 
	//	gfxbuf = malloc(vwidth * vheight * 1);
		
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Quit("Couldn't init SDL");
	}

	if (MS_CheckParm("fullscreen"))
		//surface = SDL_SetVideoMode(vwidth, vheight, 8, SDL_SWSURFACE|SDL_HWPALETTE|SDL_FULLSCREEN);
		surface = SDL_SetVideoMode(vwidth, vheight, 8, SDL_SWSURFACE|SDL_HWPALETTE|SDL_FULLSCREEN|SDL_DOUBLEBUF);
	else
		//surface = SDL_SetVideoMode(vwidth, vheight, 8, SDL_SWSURFACE|SDL_HWPALETTE);
		surface = SDL_SetVideoMode(vwidth, vheight, 8, SDL_SWSURFACE|SDL_HWPALETTE|SDL_DOUBLEBUF);
		
	if (surface == NULL) {
		SDL_Quit();
		Quit("Couldn't set 320x200 mode");
	}
	gfxbuf = surface->pixels;
	
	SDL_WM_SetCaption(GAMENAME, GAMENAME);

	SDL_ShowCursor(0);	
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
	//if (gfxbuf != NULL) {
	//	free(gfxbuf);
	//	gfxbuf = NULL;
	//}
	SDL_Quit();
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
	SDL_Color colors[256];
	int i;
	
	VL_WaitVBL(1);
	
	for (i = 0; i < 256; i++)
	{
		colors[i].r = palette[i*3+0] << 2;
		colors[i].g = palette[i*3+1] << 2;
		colors[i].b = palette[i*3+2] << 2;
	}
	SDL_SetColors(surface, colors, 0, 256);
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
	int i;
	for (i=0;i<256;i++)
	{
		palette[i*3+0] = surface->format->palette->colors[i].r >> 2;
		palette[i*3+1] = surface->format->palette->colors[i].g >> 2;
		palette[i*3+2] = surface->format->palette->colors[i].b >> 2;
	}
}

static int XKeysymToScancode(unsigned int keysym)
{
	switch (keysym) {
		case SDLK_1:
			return sc_1;
		case SDLK_2:
			return sc_2;
		case SDLK_3:
			return sc_3;
		case SDLK_4:
			return sc_4;
		case SDLK_a:
			return sc_A;
		case SDLK_b:
			return sc_B;
		case SDLK_c:
			return sc_C;
		case SDLK_e:
			return sc_E;
		case SDLK_g:
			return sc_G;
		case SDLK_h:
			return sc_H;
		case SDLK_i:
			return sc_I;
		case SDLK_l:
			return sc_L;
		case SDLK_m:
			return sc_M;
		case SDLK_n:
			return sc_N;
		case SDLK_r:
			return sc_R;
		case SDLK_t:
			return sc_T;
		case SDLK_v:
			return sc_V;
		case SDLK_y:
			return sc_Y;
		case SDLK_F8:
			return sc_F8;
		case SDLK_F9:
			return sc_F9;
		case SDLK_LEFT:
		case SDLK_KP4:
			return sc_LeftArrow;
		case SDLK_RIGHT:
		case SDLK_KP6:
			return sc_RightArrow;
		case SDLK_UP:
		case SDLK_KP8:
			return sc_UpArrow;
		case SDLK_DOWN:
		case SDLK_KP2:
			return sc_DownArrow;
		case SDLK_LCTRL:
			return sc_Control;
		case SDLK_LALT:
			return sc_Alt;
		case SDLK_LSHIFT:
			return sc_LShift;
		case SDLK_RSHIFT:
			return sc_RShift;
		case SDLK_ESCAPE:
			return sc_Escape;
		case SDLK_SPACE:
			return sc_Space;
		case SDLK_KP_ENTER:
		case SDLK_RETURN:
			return sc_Enter;
		case SDLK_TAB:
			return sc_Tab;
		case SDLK_BACKSPACE:
			return sc_BackSpace;
		case SDLK_PAUSE:
			return 0xE1;
		default:
			return sc_None;
	}
}

void INL_Update()
{
	SDL_Event event;
	
	if (SDL_PollEvent(&event)) {
		do {
			switch(event.type) {
				case SDL_KEYDOWN:
					keyboard_handler(XKeysymToScancode(event.key.keysym.sym), 1);
					break;
				case SDL_KEYUP:
					keyboard_handler(XKeysymToScancode(event.key.keysym.sym), 0);
					break;
				default:
					break;
			}
		} while (SDL_PollEvent(&event));
	}
}
