// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
//
// DESCRIPTION:
//      System interface, sound.
//
//-----------------------------------------------------------------------------

#ifndef __I_SOUND__
#define __I_SOUND__

#include "doomdef.h"
#include "sounds.h"


// Special consvar for deciding between native and fluidsynth
#ifdef HAVE_SDL
extern consvar_t cv_midibackend;
extern consvar_t cv_soundfontpath;
#endif

void* I_GetSfx (sfxinfo_t*  sfx);
void  I_FreeSfx (sfxinfo_t* sfx);


// Init at program start...
void I_StartupSound();

// ... update sound buffer and audio device at runtime...
void I_UpdateSound(void);
void I_SubmitSound(void);

// ... shut down and relase at program termination.
void I_ShutdownSound(void);


//
//  SFX I/O
//

// Starts a sound in a particular sound channel.
int
I_StartSound
( int           id,
  int           vol,
  int           sep,
  int           pitch,
  int           priority,
  int			channel);


// Stops a sound channel.
void I_StopSound(int handle);

// Called by S_*() functions
//  to see if a channel is still playing.
// Returns 0 if no longer playing, 1 if playing.
int I_SoundIsPlaying(int handle);

// Updates the volume, separation,
//  and pitch of a sound channel.
void
I_UpdateSoundParams
( int           handle,
  int           vol,
  int           sep,
  int           pitch );


//
//  MUSIC I/O
//
void I_InitMusic(void);
void I_ShutdownMusic(void);
// Volume.
void I_SetMusicVolume(int volume);
void I_SetSfxVolume(int volume);
// PAUSE game handling.
void I_PauseSong(int handle);
void I_ResumeSong(int handle);
// Registers a song handle to song data.
int I_RegisterSong(void* data,int len);
// Called by anything that wishes to start music.
//  plays a song, and when the song is done,
//  starts playing it again in an endless loop.
// Horrible thing to do, considering.
void
I_PlaySong
( int           handle,
  int           looping );
// Stops a song over 3 seconds.
void I_StopSong(int handle);
// See above (register), then think backwards
void I_UnRegisterSong(int handle);


// i_cdmus.h : cd music interface
//
extern byte    cdaudio_started;

void   I_InitCD (void);
void   I_StopCD (void);
void   I_ResumeCD (void);
void   I_ShutdownCD (void);
void   I_UpdateCD (void);
void   I_PlayCD (int track, boolean looping);
int    I_SetVolumeCD (int volume);  // return 0 on failure

#endif
//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
