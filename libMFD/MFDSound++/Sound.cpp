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
#ifdef ORB2006
    #include "MFDSoundSDK35.h"
#else
    #include "MFDSoundSDK40.h"
#endif // ORB2006
#include "Sound.hpp"
#include <sstream>

using namespace EnjoLib::MFDGoodies;

const int Sound::m_maxSlotID = 9;

Sound::Sound( const SoundMap & soundMap )
    :
    m_soundMap(soundMap)
    , m_useSoundUser(true)
{
    m_soundSystemID = -1;
    m_numReinited = 0;
}

Sound::~Sound()
{
    // Add cleanup if necessary
}

void Sound::Reinit()
{
    ++m_numReinited;
    m_soundSystemID = -1;
}

void Sound::Connect( const char * uniqueName )
{
    if ( m_soundSystemID == -1) // Connect only once
    {
        std::ostringstream ss;
        ss << m_numReinited << uniqueName;
        std::string strId( ss.str() );
        if (strId.length() > 256 )
            strId = strId.substr(0, 255); // Preventing buffer overflow
        char * cid = (char *)(strId.c_str()); // Assuming that the library doesn't modify the ID
        m_soundSystemID = ConnectMFDToOrbiterSound( cid ); // Connect, using the char ID
        if (m_soundSystemID < 0)
        {
            // Failed to connect - resign connection attempts and quit
            m_soundSystemID = -2;
            return;
        }
        for (SoundMap::const_iterator it = m_soundMap.begin(); it != m_soundMap.end(); ++it)
        {
            // Load all samples up to slot nr 8 inclusive and leave the 9th untouched
            // The library allows to load only 10 samples and the last slot will be filled dynamically
            int i = static_cast<int>(it->first);
            if ( i < m_maxSlotID )
            {
                if ( ! LoadMFDWave(m_soundSystemID,i,it->second) )
                    sprintf_s(oapiDebugString(), 255, "The sound sample '%s' has not been found in the file system. Check your SoundMap", it->second );
            }

        }
    }
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
    // Have we successfully connected and do we have a go for using sound from user?
    return m_soundSystemID >= 0 && m_useSoundUser;
}

void Sound::PlaySound(const int sample)
{
    if ( ! IsUsingSound() ) // Don't play amything, if instructed so
        return;

    BOOL success = true;
    int slot = static_cast<int>(sample);
    if ( slot >= m_maxSlotID )
    {
        // If this sound ID is equal, or exceeds the number of slots, load sample dynamically
        slot = m_maxSlotID; // load it to the last slot
        if ( sample != m_currentlyLoadedSampleOnLastSlot ) // only of if hasn't been already loaded
        {
            SoundMap::const_iterator it = m_soundMap.find(sample); // get the file corresponding to this slot
            if ( it != m_soundMap.end() ) // If found
            {
                char * file = it->second; // get the file name
                success = LoadMFDWave(m_soundSystemID, slot, file); // pass it to the library
                m_currentlyLoadedSampleOnLastSlot = sample; // don't load the same sample again
                if ( ! success )
                    sprintf_s(oapiDebugString(), 255, "The sound sample '%s' has not been found in the file system. Check your SoundMap", file );
            }
            else
                sprintf_s(oapiDebugString(), 255, "The sound sample nr %d has not been mapped in SoundMap.", sample );
        }
    }
    if ( success )
        PlayMFDWave(m_soundSystemID, slot); // play the sample in the slot
}


void Sound::PlaySoundOnce( const int sample )
{
    if (m_soundPlayOnceFlags.empty())
        for (SoundMap::const_iterator it = m_soundMap.begin(); it != m_soundMap.end(); ++it)
            m_soundPlayOnceFlags[it->first] = false;

    std::map<int, bool>::iterator it = m_soundPlayOnceFlags.find(sample);
    if ( it == m_soundPlayOnceFlags.end() )
        sprintf_s(oapiDebugString(), 255, "The sound sample nr %d has not been mapped in SoundMap.", sample );
    else
    {
        if ( ! it->second )
            PlaySound( sample );
        it->second = true;
    }
}

void Sound::ResetSoundOnce( const int sample )
{
    std::map<int, bool>::iterator it = m_soundPlayOnceFlags.find(sample);
    if ( it != m_soundPlayOnceFlags.end() )
        it->second = false;
}

void Sound::ResetSoundsOnce()
{
    for (std::map<int, bool>::iterator it = m_soundPlayOnceFlags.begin(); it != m_soundPlayOnceFlags.end(); ++it)
        it->second = false;
}

