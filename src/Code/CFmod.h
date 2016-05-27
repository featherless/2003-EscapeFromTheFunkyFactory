/*
 Copyright 2003 The Developers Alliance. All Rights Reserved.
 Copyright 2003, 2016 Jeff Verkoeyen. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#ifndef _CFMOD_H
#define _CFMOD_H

#include "fmod.h"

class CFmod
{
public:

	// Typical constructor
	CFmod();

	// This will clean up memory and close the sound system
	~CFmod();

	// This inits your CFmod class MUST BE CALLED before any Fmod functions
	bool Init(char*);

	// This takes a file name (a mp3 or wav file) and loads it into the m_pSound
	bool LoadSong(char *strName);

	// This will play the current song stored in the m_pSound (Must be loaded first)
	void PlaySong();

	//Get's position
	void StopSong();

	// This will free the memory stored in m_pSound (Song go bye bye)
	void FreeSound();

	//This will change the volume of the current track
	void Volume(int volume);

	//Toggles repeat mode
	void Repeat(int repeat);

	//Get's position
	int GetPosition();

	// This will close the FMod Sound System
	void FreeSoundSystem();

	// This will test if the sound is playing or not
	signed char IsPlaying();

	// Sets the spectrum either on or off
	void SetSpectrum(bool On);

	// Sets the spectrum either on or off
	float* GetSpectrum();

	//////////////////// Data Access Functions //////////////////// 

	// This returns the pointer to our song module
	FSOUND_SAMPLE *GetSound()
	{
		return m_pSound;
	}

	const char *GetSongName() { return FSOUND_Sample_GetName(m_pSound); }

	// This will return a string of characters for the name of the file loaded
	const char *GetFileName() { return m_strName; }

private:

	// This holds the data for the current song loaded (Like a handle)
	FSOUND_SAMPLE *m_pSound;

	// This holds the name of the current song loaded
	char m_strName[255];

	// This holds the sound channel that is returned from FSOUND_PlaySound()
	// Since we pick any free channel, we need to know which one the sound was assigned.
	int m_soundChannel;
};

#endif