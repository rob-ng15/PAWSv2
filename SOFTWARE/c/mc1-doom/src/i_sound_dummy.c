// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 2020 by Marcus Geelnard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Dummy system interface for sound.
//
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "doomdef.h"

#include <PAWSlibrary.h>

//
// SFX API.
// This is currently an empty implementation. No sound support yet.
//

// The actual lengths of all sound effects.
short *s_sfx_lengths = (short *)0x2700; // // moved to BRAM from //static short s_sfx_lengths[NUMSFX];

//
// This function loads the sound data from the WAD lump, for single sound.
//
static void* getsfx (const char* sfxname, short* len)
{
    // Get the sound data from the WAD, allocate lump in zone memory.
    char name[20];
    sprintf (name, "dp%s", sfxname);

    // Now, there is a severe problem with the sound handling, in it is not
    // (yet/anymore) gamemode aware. That means, sounds from DOOM II will be
    // requested even with DOOM shareware.
    //
    // The sound list is wired into sounds.c, which sets the external variable.
    // I do not do runtime patches to that variable. Instead, we will use a
    // default sound for replacement.
    int sfxlump;
    if (W_CheckNumForName (name) == -1)
        sfxlump = W_GetNumForName ("dppistol");
    else
        sfxlump = W_GetNumForName (name);

    int size = W_LumpLength (sfxlump);

    unsigned char* sfx = (unsigned char*)W_CacheLumpNum (sfxlump, PU_STATIC);

    *len = (short)size;
    return (void *)sfx;
}

void I_InitSound (void)
{
    // Initialize external data (all sounds) at start, keep static.
    // Pre-cache all sounds effects.
    for (int i = 1; i < NUMSFX; i++)
    {
        // Alias? Example is the chaingun sound linked to pistol.
        if (!S_sfx[i].link)
        {
            // Load data from WAD file.
            S_sfx[i].data = getsfx (S_sfx[i].name, &s_sfx_lengths[i]);
        }
        else
        {
            // Previously loaded already?
            S_sfx[i].data = S_sfx[i].link->data;
            s_sfx_lengths[i] =
                s_sfx_lengths[(S_sfx[i].link - S_sfx) /
                              sizeof (sfxinfo_t)];  // Is this correct?
        }
    }
}

void I_ShutdownSound (void)
{
}

void I_SetSfxVolume (int volume)
{
    (void)volume;
}

void I_SetMusicVolume (int volume)
{
    (void)volume;
}

// LOOK FOR THE PC SPEAKER FORM OF THE SOUND EFFECT
int I_GetSfxLumpNum (sfxinfo_t* sfx)
{
    char namebuf[9];
    sprintf (namebuf, "dp%s", sfx->name);
    return W_GetNumForName (namebuf);
}

unsigned char __lastchannel = 1;
int I_StartSound (int id, int vol, int sep, int pitch, int priority)
{
    __lastchannel = 3 - __lastchannel;      // MOVE TO NEXT CHANNEL
    beep( __lastchannel, 0, 0, 0 );         // STOP CHANNEL

    sample_upload( __lastchannel,  s_sfx_lengths[id] - 4, S_sfx[id].data + 4 );
    beep( __lastchannel, WAVE_SAMPLE, 1, 16 );
    return id;
}

void I_StopSound (int handle)
{
    (void)handle;
}

void I_StopAllSounds ()
{
    beep( CHANNEL_BOTH, WAVE_SQUARE, 0, 0 );
}

int I_SoundIsPlaying (int handle)
{
    (void)handle;
    return 0;
}

void I_UpdateSoundParams (int handle, int vol, int sep, int pitch)
{
    (void)handle;
    (void)vol;
    (void)sep;
    (void)pitch;
}

void I_UpdateSound (void)
{
}

void I_SubmitSound (void)
{
}

//
// MUSIC API.
// This is currently an empty implementation. No music support yet.
//

void I_InitMusic (void)
{
}

void I_ShutdownMusic (void)
{
}

void I_PlaySong (int handle, int looping)
{
    (void)handle;
    (void)looping;
}

void I_PauseSong (int handle)
{
    (void)handle;
}

void I_ResumeSong (int handle)
{
    (void)handle;
}

void I_StopSong (int handle)
{
    (void)handle;
}

void I_UnRegisterSong (int handle)
{
    (void)handle;
}

int I_RegisterSong (void* data)
{
    (void)data;
    return 1;
}

int I_QrySongPlaying (int handle)
{
    (void)handle;
    return 0;
}
