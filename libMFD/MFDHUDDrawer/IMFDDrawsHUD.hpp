// ==============================================================
// ORBITER AUX LIBRARY: Support for HUD drawing from MFD context
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Warning! This library is a HACK and it may stop working
// in future versions of Orbiter API if the library isn't
// maintained anymore. Note however that it's very easy to do so.
//
// Copyright (C) 2008 Steve "agentgonzo" Arch - most of work
// Copyright (C) 2012 Szymon "Enjo" Ender - created a framework
//
//                         All rights reserved
//
// MFDHUDDrawer is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// MFDHUDDrawer is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with MFDHUDDrawer. If not, see
// <http://www.gnu.org/licenses/>.
// ==============================================================

#ifndef IMFDDrawsHUD_HPP
#define IMFDDrawsHUD_HPP

#include "Orbitersdk.h"


namespace EnjoLib
{
namespace MFDGoodies
{
/// Interface for MFD that is supposed to draw on HUD
/**
Make this a base class of your MFD
*/
class IMFDDrawsHUD
{
public:
    /// Ctor
    IMFDDrawsHUD();
    /// Dtor
    virtual ~IMFDDrawsHUD();

    /// Should the MFD draw on HUD now?
    /**
    Could return false, for example when the user switched drawing on HUD through an MFD button
    */
    virtual bool ShouldDrawHUD() const = 0;
#ifndef ORB2006
    /// Provides drawing context for the MFD
    virtual void DrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp) = 0;
#endif
    /// Only for backward compatibility (Orbiter 2006), therefore may be left unimplemented.
    /**
    If you need to provide backward compatibility, the code executed by this method
    should be equal to the second drawing method.
    */
    virtual void DrawHUD(int mode, const HUDPAINTSPEC *hps, HDC hDC);

protected:
private:
};
}
}

#endif // IMFDDrawsHUD_HPP
