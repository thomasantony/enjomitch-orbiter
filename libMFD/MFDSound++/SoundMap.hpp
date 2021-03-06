// ==============================================================
//         ORBITER AUX LIBRARY: Dan's MFD Sound C++ wrapper
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Copyright (C) 2011      Szymon "Enjo" Ender
//                         All rights reserved
//
// SoundMap - Maps a sample id (an enum) onto a filename of sound sample
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

#ifndef SOUNDMAP_HPP_INCLUDED
#define SOUNDMAP_HPP_INCLUDED

#include <map>
#include <string>

namespace EnjoLib
{
namespace MFDGoodies
{
/// Sound map
/** Maps a sample id (an enum) onto a filename of sound sample.
You need to derive from this class and put your mapping in constructor.
Then you need to store the derived map and pass it to Sound class.
*/
	class SoundMap : public std::map<int, std::string>
{
public:
    /// Default constructor
    /** The actual mapping is performed here like in the following example:
    <p>
    const std::string dir = "Sound/LaunchMFDEnjo/";
    AddSample(HALF_ORBITAL_VEL, dir + "HalfOV.wav");<br>
    AddSample(TARGET_MANUAL,    dir + "ManualTarget.wav");
    </p>
    */
    SoundMap();

protected:
    void AddSample(int enumId, const std::string & relativeFileName);
};
}
}

#endif // SOUNDMAP_HPP_INCLUDED
