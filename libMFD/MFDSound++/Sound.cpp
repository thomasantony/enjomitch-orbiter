// ==============================================================
//         ORBITER AUX LIBRARY: Dan's MFD Sound C++ wrapper
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Copyright (C) 2011      Szymon "Enjo" Ender
//                         All rights reserved
//
// Sound - Hides all the dynamic sample loading
//
// Multiple Vessels Support is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Multiple Vessels Support is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Multiple Vessels Support. If not, see
// <http://www.gnu.org/licenses/>.
// ==============================================================

#include "Orbitersdk.h"
#include "Sound.hpp"

#include <Util/FileUtils.hpp>

#include <mmsystem.h>
#include <stdio.h>  /* defines FILENAME_MAX */
#include <direct.h>
#include <sstream>

using namespace EnjoLib;
using namespace EnjoLib::MFDGoodies;

Sound::Sound( const SoundMap & soundMap )
    :
    m_soundMap(soundMap)
    , m_useSoundUser(true)
{
}

Sound::~Sound()
{
    // Add cleanup if necessary
}

void Sound::SetUseSound( const bool use )
{
    this->m_useSoundUser = use;
}

void Sound::SwitchUseSound()
{
    m_useSoundUser = ! m_useSoundUser;
}

bool Sound::IsUsingSound()
{
    // Do we have a go for using sound from user?
    return m_useSoundUser;
}

void Sound::PlayWave(const int sample)
{
    if ( ! IsUsingSound() ) // Don't play amything, if instructed so
        return;

	SoundMap::const_iterator it = m_soundMap.find(sample);
	char cCurrentPath[FILENAME_MAX];
	if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
	{
		sprintf_s(oapiDebugString(), 255, "Couldn't get current dir" );
		return;
	}

	std::string absPath = std::string(cCurrentPath) + '/' + it->second;
	if (! FileUtils().FileExists(absPath))
	{
		sprintf_s(oapiDebugString(), FILENAME_MAX, "Couldn't load sample = %s. Reinstall your addon.", absPath);
		return;
	}
	::PlaySoundA((LPCSTR) absPath.c_str(), NULL, SND_FILENAME | SND_ASYNC);
}


void Sound::PlayWaveOnce( const int sample )
{
    if (m_soundPlayOnceFlags.empty())
        for (SoundMap::const_iterator it = m_soundMap.begin(); it != m_soundMap.end(); ++it)
            m_soundPlayOnceFlags[it->first] = false;

    std::map<int, bool>::iterator it = m_soundPlayOnceFlags.find(sample);
    if ( it == m_soundPlayOnceFlags.end() )
		sprintf_s(oapiDebugString(), 255, "The sound sample number %d has not been mapped in SoundMap.", sample );
    else
    {
        if ( ! it->second )
            PlayWave( sample );
        it->second = true;
    }
}

void Sound::ResetWaveOnce( const int sample )
{
    std::map<int, bool>::iterator it = m_soundPlayOnceFlags.find(sample);
    if ( it != m_soundPlayOnceFlags.end() )
        it->second = false;
}

void Sound::ResetWavesOnce()
{
    for (std::map<int, bool>::iterator it = m_soundPlayOnceFlags.begin(); it != m_soundPlayOnceFlags.end(); ++it)
        it->second = false;
}

