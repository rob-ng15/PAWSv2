/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2021 Marcus Geelnard

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

#include "quakedef.h"
#include "errno.h"

#include <stdint.h>
#include <sys/time.h>

#include <PAWSlibrary.h>

// Memory config.
#define HEAP_SIZE_MB 16

qboolean isDedicated;

unsigned char PAWSKEYlookup[] = {
    0x00, K_F9, 0x00, K_F5, K_F3, K_F1, K_F2, K_F12, 0x00, K_F10, K_F8, K_F6, K_F4, K_TAB, 0x60, 0x00,     						// 0x00 - 0x0f
    0x00, K_ALT, 0x00, 0x00, 0x00, 'q', '1', 0x00, 0x00, 0x00, 'z', 's', 'a', 'w', '2', 0x00,                                	// 0x10 - 0x1f
    0x00, 'c', 'x', 'd', 'e', '4', '3', 0x00, 0x00, ' ', 'v', 'f', 't', 'r', '5', 0x00,                                         // 0x20 - 0x2f
    0x00, 'n', 'b', 'h', 'g', 'y', '6', 0x00, 0x00, 0x00, 'm', 'j', 'u', '7', '8', 0x00,                                        // 0x30 - 0x3f
    0x00, ',', 'k', 'i', 'o', '0', '9', 0x00, 0x00, '.', '/', 'l', ';', 'p', '-', 0x00,                              		// 0x40 - 0x4f
    0x00, 0x00, 0x27, 0x00, '[', '=', 0x00, 0x00, 0x00, K_SHIFT, K_ENTER, ']', 0x00, '~', 0x00, 0x00,             			// 0x50 - 0x5f
    0x00, 0x5c, 0x00, 0x00, 0x00, 0x00, K_BACKSPACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                    	// 0x60 - 0x6f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, K_ESCAPE, K_PAUSE, K_F11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               		// 0x70 - 0x7f
    0x00, 0x00, 0x00, K_F7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                           	// 0x80 - 0x8f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0x90 - 0x9f
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xa0 - 0xaf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xb0 - 0xbf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xc0 - 0xcf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xd0 - 0xdf
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                             // 0xe0 - 0xef
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00                              // 0xf0 - 0xff
};


static int Sys_TranslateKey (unsigned keycode)
{
    switch( keycode ) {
        case 0x111: return K_ALT;
        case 0x114: return K_CTRL;
		case 0x169: return K_END;
        case 0x16b: return K_LEFTARROW;
		case 0x16c: return K_HOME;
		case 0x170: return K_INS;
		case 0x171: return K_DEL;
        case 0x172: return K_DOWNARROW;
		case 0x17d: return K_PGUP;
        case 0x175: return K_UPARROW;
        case 0x174: return K_RIGHTARROW;
		case 0x17a: return K_PGDN;
        default:    return keycode & 0x100 ? 0 : PAWSKEYlookup[ keycode & 0xff ];
    }
}

static qboolean Sys_PollKeyEvent (void)
{
	unsigned keycode;
	int quake_key;

	// Check if we have any new keycode from the keyboard.
	if (!ps2_event_available())
		return false;

	// Get the next keycode.
	keycode = ps2_event_get();

	// Translate the MC1 keycode to a Quake keycode.
	quake_key = Sys_TranslateKey (keycode & 0x1ff);
	if (quake_key != 0)
	{
		Key_Event (quake_key, (keycode & 0x200) ? true : false);
	}
	return true;
}

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES 10
FILE *sys_handles[MAX_HANDLES];

int findhandle (void)
{
	int i;

	for (i = 1; i < MAX_HANDLES; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength (FILE *f)
{
	int pos;
	int end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead (char *path, int *hndl)
{
	FILE *f;
	int i;

	i = findhandle ();

	f = fopen (path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;

	return filelength (f);
}

int Sys_FileOpenWrite (char *path)
{
	FILE *f;
	int i;

	i = findhandle ();

	f = fopen (path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path, strerror (errno));
	sys_handles[i] = f;

	return i;
}

void Sys_FileClose (int handle)
{
	fclose (sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void Sys_FileSeek (int handle, int position)
{
	fseek (sys_handles[handle], position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
	return fread (dest, 1, count, sys_handles[handle]);
}

int Sys_FileWrite (int handle, void *data, int count)
{
	return fwrite (data, 1, count, sys_handles[handle]);
}

int Sys_FileTime (char *path)
{
	FILE *f;

	f = fopen (path, "rb");
	if (f)
	{
		fclose (f);
		return 1;
	}

	return -1;
}

void Sys_mkdir (char *path)
{
}

/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
}

void Sys_Error (char *error, ...)
{
	va_list argptr;

	fprintf (stderr,"Sys_Error: ");
	va_start (argptr, error);
	vfprintf (stderr,error, argptr);
	va_end (argptr);
	fprintf (stderr,"\n");

	exit (1);
}

void Sys_Printf (char *fmt, ...)
{
	va_list argptr;

	va_start (argptr, fmt);
	vfprintf (stderr,fmt, argptr);
	va_end (argptr);
}

void Sys_Quit (void)
{
	exit (0);
}

double Sys_FloatTime (void)
{
#if 1
	// MRISC32 simulator timing: Use gettimeofday().
	static qboolean s_first = true;
	static struct timeval s_t0;
	struct timeval t;
	float t_sec;

	if (s_first)
	{
		gettimeofday (&s_t0, NULL);
		s_first = false;
	}
	gettimeofday (&t, NULL);
	t_sec = (float)(t.tv_sec - s_t0.tv_sec) +
			0.000001F * (float)(t.tv_usec - s_t0.tv_usec);
	return (double)t_sec;
#else
	// MC1 timing: Use CLKCNTHI:CLKCNTLO MMIO registers directly.
	static qboolean s_first = true;
	static double s_inv_clk;
	uint32_t hi_old, hi, lo;
	uint64_t cycles;
	double t;

	// Get 1 / cycles per s.
	if (s_first)
	{
		s_inv_clk = 1.0 / (double)GET_MMIO (CPUCLK);
		s_first = false;
	}

	// Get number of CPU cycles (64-bit number).
	hi = GET_MMIO (CLKCNTHI);
	do
	{
		hi_old = hi;
		lo = GET_MMIO (CLKCNTLO);
		hi = GET_MMIO (CLKCNTHI);
	} while (hi != hi_old);

	cycles = (((uint64_t)hi) << 32) | (uint64_t)lo;
	return s_inv_clk * (double)cycles;
#endif
}

char *Sys_ConsoleInput (void)
{
	return NULL;
}

void Sys_Sleep (void)
{
}

void Sys_SendKeyEvents (void)
{
	while (Sys_PollKeyEvent ())
		;
}

void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}

//=============================================================================

void main (int argc, char **argv)
{
	static quakeparms_t parms;
	double oldtime, newtime;
	float time;

	parms.memsize = HEAP_SIZE_MB * 1024 * 1024;
	parms.membase = malloc (parms.memsize);
	parms.basedir = "/DEMO";

	COM_InitArgv (argc, argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	fprintf (stderr,"Host_Init\n");
	Host_Init (&parms);

	oldtime = Sys_FloatTime () - 0.1;
	while (1)
	{
		// find time spent rendering last frame
		newtime = Sys_FloatTime ();
		time = (float)(newtime - oldtime);

		if (time > (sys_ticrate.value * 2.0F))
			oldtime = newtime;
		else
			oldtime += (double)time;

		Host_Frame (time);
	}
}
