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

int __lastchannel = 1; int __handles[3] = { 0, 0, 0 };
int I_StartSound (int id, int vol)
{
    unsigned char volatile *AUDIO_REGS_B = (unsigned char volatile *)AUDIO_REGS; short volatile *AUDIO_REGS_H = (short volatile *)AUDIO_REGS;
    unsigned char volatile *DMA_REGS_B = (unsigned char volatile *)DMA_REGS;

    __lastchannel = 3 - __lastchannel; __handles[ __lastchannel ] = id;                                                             // MOVE TO NEXT CHANNEL, STORE THE SOUND ID
    AUDIO_REGS[ 0x00 ] = 0; AUDIO_REGS_H[ 0x02 ] = 0; AUDIO_REGS_B[ 0x06 ] = __lastchannel;                                         // STOP THE CHANNEL
    AUDIO_REGS_B[ 0x08 ] = __lastchannel; DMA_REGS[1] = ( __lastchannel == 1 ) ? 0xe00a : 0xe00c;                                   // SELECT THE CHANNEL
    DMA_REGS[0] = (int)S_sfx[id].data + 4;  DMA_REGS[2] = s_sfx_lengths[id] - 4; DMA_REGS_B[0x0c] = 1;                              // TRANSFER THE SAMPLE VIA DMA
    AUDIO_REGS[ 0x00 ] = 0x10000 | WAVE_SAMPLE | WAVE_SQUARE; AUDIO_REGS_H[ 0x02 ] = 8; AUDIO_REGS_B[ 0x06 ] = __lastchannel;         // START THE SAMPLE ( 8 counts per sample ~ 140 Hz )
    AUDIO_REGS_B[ ( __lastchannel == 1 ) ? 0x10 : 0x12 ] = 7 - ( ( 8 - vol ) >> 1 );                                                // SET THE VOLUME, CONVERT FROM 8 to 1 to PAWS 7 - 0

    return id;
}

void I_StopSound (int handle)
{
    unsigned char volatile *AUDIO_REGS_B = (unsigned char volatile *)AUDIO_REGS; short volatile *AUDIO_REGS_H = (short volatile *)AUDIO_REGS;
    unsigned char volatile *DMA_REGS_B = (unsigned char volatile *)DMA_REGS;

    if( __handles[ 1 ] == handle ) { AUDIO_REGS[ 0x00 ] = 0; AUDIO_REGS_H[ 0x02 ] = 0; AUDIO_REGS_B[ 0x06 ] = 1; }                  // STOP IF HANDLE IS PLAYING LEFT
    if( __handles[ 2 ] == handle ) { AUDIO_REGS[ 0x00 ] = 0; AUDIO_REGS_H[ 0x02 ] = 0; AUDIO_REGS_B[ 0x06 ] = 2; }                  // STOP IF HANDLE IS PLAYING RIGHT
}

void I_StopAllSounds ()
{
    AUDIO_REGS[ 0x00 ] = 0; AUDIO_REGS[ 0x01 ] = 0x30000;                                                                           // STOP ALL AUDIO
}

int I_SoundIsPlaying (int handle)
{
    unsigned char volatile *DMA_REGS_B = (unsigned char volatile *)DMA_REGS, *AUDIO_REGS_B = (unsigned char volatile *)AUDIO_REGS;
    return ( __handles[ 1 ] == handle ) ? AUDIO_REGS_B[ 0x10 ] : ( __handles[ 2 ] == handle ) ? AUDIO_REGS_B[ 0x12 ] : 0;;          // IS HANDLE STILL PLAYING ON A CHANNEL?
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
