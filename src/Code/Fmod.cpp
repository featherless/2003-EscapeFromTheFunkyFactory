#include <windows.h>
#include "CFmod.h"								// Include our header file for Fmod prototypes

///////////////////////////////// CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is if you want to create a global variable and not init the sound system
/////
///////////////////////////////// CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CFmod::CFmod()
{
	// Clear the file name
	memset(m_strName, 0, sizeof(char));

	// Set the current song module to NULL
	m_pSound = NULL;
}


///////////////////////////////// ~CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will clean up memory and close the sound system
/////
///////////////////////////////// ~CFMOD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CFmod::~CFmod()
{
	// Free the sound pointer that loaded our song if it's valid
	FreeSound();

	// Now free the whole sound system
	FreeSoundSystem();
}


///////////////////////////////// INITIALIZE SOUND SYSTEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This inits the sound system and should be called before any Fmod functions
/////
///////////////////////////////// INITIALIZE SOUND SYSTEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFmod::Init(char *strName)
{
	// ==========================================================================================
	// CHECK DLL VERSION
	// ==========================================================================================

	// First we want to make sure the DLL that we are using is the correct one we need.
	// Below we check if the one we are using is less than the one we need.

	// Make sure we are using the correct DLL version
	
	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		return false;
	}

	// ==========================================================================================
	// INIT SOUND SYSTEM
	// ==========================================================================================

	// Now that we know we have the right DLL, we need to initialize the FMod Sound System.
	// We want our mix rate to be to be 44100mhz (Good CD Quality), and we pass in 32 for
	// the max software channels, then FSOUND_INIT_GLOBALFOCUS to show we want it to play
	// no matter what window is in focus.
	
	// Initialize our FMod Sound System with good quality sound and a global focus
	if (!FSOUND_Init(44100, 32, FSOUND_INIT_GLOBALFOCUS))
	{
		return false;
	}

	// Return the result from when we attempt to load the song
	return LoadSong(strName);
}


///////////////////////////////// LOAD SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a file name (a wav or mp3 file) and loads it into the m_pSound
/////
///////////////////////////////// LOAD SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CFmod::LoadSong(char *strName)
{
	// ==========================================================================================
	// LOAD SONG
	// ==========================================================================================

	// If strName is invalid, can't load song
	if(!strName)
		return false;

	// Everything should be up and running with the sound system, so let's load a song.
	// To load a song we just pass in the file name to FSOUND_Sample_Load().
	// This returns a FSOUND_SAMPLE pointer.  We use this pointer to query and play the song.

	// Load the desired song and store the pointer to the sound.
	m_pSound = FSOUND_Sample_Load(FSOUND_FREE, strName, 0, 0);

	// Make sure we got a valid sound pointer
	if (!m_pSound)
	{
		return false;
	}

	// Set the file name to the name passed in
	strcpy(m_strName, strName);

	// Return a success!
	return true;
}

///////////////////////////////// PLAY SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This plays the current sound that is loaded on any free channel
/////
///////////////////////////////// PLAY SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

// This will play the current song stored in the m_pSound (Must be loaded first)
void CFmod::PlaySong()
{
	// ==========================================================================================
	// PLAY SONG
	// ==========================================================================================

	// We should have a pointer to a sound that will allow us to play the song.
	// All we need to do is pass in the module to FSOUND_PlaySound().  We pass
	// in SOUND_FREE to say we don't care which channel it's on, just pick any
	// free channel.  The next parameter is the pointer to our sound.

	// Play our song that we loaded, then get the sound channel it chose to assign it too.
	m_soundChannel = FSOUND_PlaySound(FSOUND_FREE, m_pSound);				

}

///////////////////////////////// STOP SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This stops the current sound
/////
///////////////////////////////// STOP SONG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

// This will play the current song stored in the m_pSound (Must be loaded first)
void CFmod::StopSong()
{
	FSOUND_StopSound(m_soundChannel);				
}

///////////////////////////////// FREE SOUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will free the memory stored in m_pSound (Song go bye bye)
/////
///////////////////////////////// FREE SOUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::FreeSound()
{
	// Free the sound in the current soundChannel
	FSOUND_StopSound(m_soundChannel);
}


///////////////////////////////// VOLUME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will change the volume of the current song
/////
///////////////////////////////// VOLUME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::Volume(int volume)
{
	FSOUND_SetVolume(m_soundChannel, volume);
}

/////////////////////////////// ISPLAYING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will change the environment of everything
/////
/////////////////////////////// ISPLAYING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

signed char CFmod::IsPlaying()
{
	return FSOUND_IsPlaying(m_soundChannel);
}


///////////////////////////////// REPEAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	2=Repeat  1=No Repeat
/////
///////////////////////////////// REPEAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::Repeat(int repeat)
{
	FSOUND_SetLoopMode(m_soundChannel, repeat);
}


////////////////////////////// SETSPECTRUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Turns the spectrum analyzer on or off
/////
////////////////////////////// SETSPECTRUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::SetSpectrum(bool On)
{
	FSOUND_DSP_SetActive(FSOUND_DSP_GetFFTUnit(), On);
}


////////////////////////////// GETSPECTRUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Returns the spectrum array
/////
////////////////////////////// GETSPECTRUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float* CFmod::GetSpectrum()
{
	return FSOUND_DSP_GetSpectrum();
}

///////////////////////////////// GETPOSITION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	gets position of song
/////
///////////////////////////////// GETPOSITION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CFmod::GetPosition()
{
	return FSOUND_GetCurrentPosition(m_soundChannel);
}


///////////////////////////////// FREE SOUND SYSTEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will close the FMod Sound System
/////
///////////////////////////////// FREE SOUND SYSTEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CFmod::FreeSoundSystem()
{
	// Close and turn off the FMod Sound System.
	FSOUND_Close();
}