/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// vid_sdl2.c -- SDL2 video driver

// TODO(m): Implement me!

#include <SDL2/SDL.h>

#include "quakedef.h"
#include "d_local.h"

viddef_t	vid;				// global video state

#define	BASEWIDTH	320
#define	BASEHEIGHT	200
#define FULLSCREEN	0
#define VSYNC		1
#define GRABMOUSE	0

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];

static unsigned int s_palette[256];

static byte* s_vid_buffer;
static short* s_zbuffer;
static byte* s_surfcache;

static SDL_Window* s_window;
static SDL_Renderer* s_renderer;
static SDL_Texture* s_texture;

static void initFatalError (const char *message)
{
	fprintf (stderr, "FAIL: %s\n", message);
	exit (EXIT_FAILURE);
}

static unsigned int color_to_argb8888 (unsigned int r,
									   unsigned int g,
									   unsigned int b)
{
	return 0xff000000u | (r << 16) | (g << 8) | b;
}

static int translatekey (SDL_Keysym *key)
{
	int rc;

	switch (key->scancode)
	{
		case SDL_SCANCODE_LEFT:
			rc = K_LEFTARROW;
			break;
		case SDL_SCANCODE_RIGHT:
			rc = K_RIGHTARROW;
			break;
		case SDL_SCANCODE_DOWN:
			rc = K_DOWNARROW;
			break;
		case SDL_SCANCODE_UP:
			rc = K_UPARROW;
			break;
		case SDL_SCANCODE_ESCAPE:
			rc = K_ESCAPE;
			break;
		case SDL_SCANCODE_RETURN:
		case SDL_SCANCODE_EQUALS:
			rc = K_ENTER;
			break;
		case SDL_SCANCODE_TAB:
			rc = K_TAB;
			break;
		case SDL_SCANCODE_F1:
			rc = K_F1;
			break;
		case SDL_SCANCODE_F2:
			rc = K_F2;
			break;
		case SDL_SCANCODE_F3:
			rc = K_F3;
			break;
		case SDL_SCANCODE_F4:
			rc = K_F4;
			break;
		case SDL_SCANCODE_F5:
			rc = K_F5;
			break;
		case SDL_SCANCODE_F6:
			rc = K_F6;
			break;
		case SDL_SCANCODE_F7:
			rc = K_F7;
			break;
		case SDL_SCANCODE_F8:
			rc = K_F8;
			break;
		case SDL_SCANCODE_F9:
			rc = K_F9;
			break;
		case SDL_SCANCODE_F10:
			rc = K_F10;
			break;
		case SDL_SCANCODE_F11:
			rc = K_F11;
			break;
		case SDL_SCANCODE_F12:
			rc = K_F12;
			break;

		case SDL_SCANCODE_BACKSPACE:
		case SDL_SCANCODE_DELETE:
			rc = K_BACKSPACE;
			break;

		case SDL_SCANCODE_PAUSE:
			rc = K_PAUSE;
			break;

		case SDL_SCANCODE_KP_MINUS:
		case SDL_SCANCODE_MINUS:
			rc = '-';
			break;

		case SDL_SCANCODE_LSHIFT:
		case SDL_SCANCODE_RSHIFT:
			rc = K_SHIFT;
			break;

		case SDL_SCANCODE_LCTRL:
		case SDL_SCANCODE_RCTRL:
			rc = K_CTRL;
			break;

		case SDL_SCANCODE_LALT:
		case SDL_SCANCODE_LGUI:
		case SDL_SCANCODE_RALT:
		case SDL_SCANCODE_RGUI:
			rc = K_ALT;
			break;

		default:
			rc = key->sym;
			break;
	}

	return rc;
}

static void handleevent (SDL_Event *sdlevent)
{
	Uint8 buttonstate;

	switch (sdlevent->type)
	{
		case SDL_KEYDOWN:
			Key_Event (translatekey (&sdlevent->key.keysym), true);
			break;

		case SDL_KEYUP:
			Key_Event (translatekey (&sdlevent->key.keysym), false);
			break;

#if 0
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			buttonstate = SDL_GetMouseState (NULL, NULL);
			event.type = ev_mouse;
			event.data1 = 0 | (buttonstate & SDL_BUTTON (1) ? 1 : 0) |
						  (buttonstate & SDL_BUTTON (2) ? 2 : 0) |
						  (buttonstate & SDL_BUTTON (3) ? 4 : 0);
			event.data2 = event.data3 = 0;
			D_PostEvent (&event);
			break;

		case SDL_MOUSEMOTION:
			event.type = ev_mouse;
			event.data1 = 0 |
						  (sdlevent->motion.state & SDL_BUTTON (1) ? 1 : 0) |
						  (sdlevent->motion.state & SDL_BUTTON (2) ? 2 : 0) |
						  (sdlevent->motion.state & SDL_BUTTON (3) ? 4 : 0);
			event.data2 = sdlevent->motion.xrel << 4;
			event.data3 = -(sdlevent->motion.yrel << 4);
			D_PostEvent (&event);
			break;
#endif

		case SDL_QUIT:
			// Can this be done in a nicer way?
			exit (0);
	}
}

void	VID_SetPalette (unsigned char *palette)
{
	for (int i = 0; i < 256; ++i)
	{
		unsigned int r = (unsigned int)*palette++;
		unsigned int g = (unsigned int)*palette++;
		unsigned int b = (unsigned int)*palette++;
		s_palette[i] = color_to_argb8888 (r, g, b);
	}
}

void	VID_ShiftPalette (unsigned char *palette)
{
	VID_SetPalette (palette);
}

void	VID_Init (unsigned char *palette)
{
	// Video size.
	int video_w = BASEWIDTH;
	int video_h = BASEHEIGHT;

	// Window size: Avoid silly small window sizes.
	int window_w = 1024;
	int window_h = (window_w * video_h) / video_w;

	// Should we open the window in fullscreen mode?
	Uint32 window_flags = 0;
	if (FULLSCREEN)
		window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	else
		window_flags |= SDL_WINDOW_RESIZABLE;

	// Create the window.
	s_window = SDL_CreateWindow ("MC1-QUAKE",
								 SDL_WINDOWPOS_UNDEFINED,
								 SDL_WINDOWPOS_UNDEFINED,
								 window_w,
								 window_h,
								 window_flags);
	if (s_window == NULL)
		initFatalError ("Couldn't create SDL window");

	// Create the renderer.
	s_renderer = SDL_CreateRenderer (
		s_window, -1, VSYNC ? SDL_RENDERER_PRESENTVSYNC : 0);
	if (s_renderer == NULL)
		initFatalError ("Couldn't create SDL renderer");
	SDL_RenderSetLogicalSize (s_renderer, video_w, video_h);

	// Create the texture.
	s_texture = SDL_CreateTexture (s_renderer,
								   SDL_PIXELFORMAT_ARGB8888,
								   SDL_TEXTUREACCESS_STREAMING,
								   video_w,
								   video_h);
	if (s_texture == NULL)
		initFatalError ("Couldn't create SDL texture");

	// Configure the mouse.
	if (GRABMOUSE || FULLSCREEN)
	{
		SDL_CaptureMouse (SDL_TRUE);
		SDL_SetRelativeMouseMode (SDL_TRUE);
	}
	SDL_ShowCursor (SDL_DISABLE);

	// Allocate video working memory.
	s_vid_buffer = (byte *)malloc (video_w * video_h * sizeof (byte));
	s_zbuffer = (short *)malloc (video_w * video_h * sizeof (short));
	size_t surfcache_size = 256 * 1024 * sizeof (byte);
	s_surfcache = (byte *)malloc (surfcache_size);

	vid.maxwarpwidth = vid.width = vid.conwidth = video_w;
	vid.maxwarpheight = vid.height = vid.conheight = video_h;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = s_vid_buffer;
	vid.rowbytes = vid.conrowbytes = video_w;

	d_pzbuffer = s_zbuffer;
	D_InitCaches (s_surfcache, surfcache_size);

	VID_SetPalette (palette);
}

void	VID_Shutdown (void)
{
	free (s_surfcache);
	free (s_zbuffer);
	free (s_vid_buffer);

	SDL_DestroyTexture (s_texture);
	SDL_DestroyRenderer (s_renderer);
	SDL_DestroyWindow (s_window);
	SDL_Quit ();
}

void	VID_Update (vrect_t *rects)
{
	// Copy the internal screen to the SDL texture, converting the 8-bit indexed
	// pixels to 32-bit ARGB.
	void *pixels;
	int pitch;
	if (SDL_LockTexture (s_texture, NULL, &pixels, &pitch) != 0)
	{
		fprintf (stderr, "VID_Update: Unable to lock texture\n");
		return;
	}
	const unsigned char *src = (const unsigned char *)s_vid_buffer;
	unsigned int *dst = (unsigned int *)pixels;
	for (unsigned y = 0; y < vid.height; ++y)
	{
		for (unsigned x = 0; x < vid.width; ++x)
			dst[x] = s_palette[src[x]];
		src += vid.width;
		dst += pitch / 4;
	}
	SDL_UnlockTexture (s_texture);

	// Render the texture.
	SDL_RenderClear (s_renderer);
	SDL_RenderCopy (s_renderer, s_texture, NULL, NULL);
	SDL_RenderPresent (s_renderer);
}

/*
================
D_BeginDirectRect
================
*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}


/*
================
D_EndDirectRect
================
*/
void D_EndDirectRect (int x, int y, int width, int height)
{
}


void IN_Init (void)
{
}

void IN_Shutdown (void)
{
}

void IN_Commands (void)
{
}

void IN_Move (usercmd_t *cmd)
{
}

void Sys_SendKeyEvents (void)
{
	SDL_Event e;
	while (SDL_PollEvent (&e))
		handleevent (&e);
}
