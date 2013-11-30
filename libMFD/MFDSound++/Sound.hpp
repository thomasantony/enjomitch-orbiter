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

#ifndef SOUND_HPP_INCLUDED
#define SOUND_HPP_INCLUDED

#include "SoundMap.hpp"
#include <string>

namespace EnjoLib
{
namespace MFDGoodies
{
/// Sound class
/**
A handy C++ wrapper over Dan's MFD sound library.
The sound is played by passing SoundSampleIDEnum to PlaySound().
The sound samples for this class should be defined in SoundMap.
\warning To enable the sound, you need to call Connect() from MyMFD::Update() method!
*/
class Sound
{
public:
    /// Default constructor
    /** Accepts the SoundMap derived map.
    \param soundMap
    */
    Sound( const SoundMap & soundMap );
    /// Destructor
    virtual ~Sound();

    void Reinit();

    /// Plays sound sample
    /** Plays a given sound only if the object hasn't been instructed not to play the sound
    by calling SetUseSound() with false argument.
    \param sample - sound sample defined in enum and mapped to file in SoundMap
    */
    void PlaySound( const int sample );
    void PlaySoundOnce( const int sample );
    void ResetSoundOnce( const int sample );
    void ResetSoundsOnce();

    /// Sets sound to using or not using
    /** Sets sound to using or not using
    \param use - true to use, false not to use
    */
    void SetUseSound( const bool use );

    /// Switches between using and not using sound
    /** A convenience method created, so that you don't need to store the choice externally */
    void SwitchUseSound();

    /// Connect the MFD to OrbitrSound system
    /** To actually enable the sound, this method must be called from MyMFD::Update().
    It's internals are executed only once.
    \param uniqueName - a unique ID of your MFD. Can be anything that you think is unique enough
    */
    void Connect( const char * uniqueName );

private:
	bool IsUsingSound();

    const static int m_maxSlotID;
    const SoundMap & m_soundMap;
    std::map<int, bool> m_soundPlayOnceFlags;
    int m_soundSystemID;
    int m_numReinited;
    int m_currentlyLoadedSampleOnLastSlot;
    bool m_useSoundUser;
};
}
}

#endif // SOUND_HPP_INCLUDED
