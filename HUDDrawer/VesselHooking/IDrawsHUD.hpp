// ==============================================================
//                ORBITER AUX LIBRARY: HUDDrawer
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Allows modules (MFDs or plain modules) to draw on HUD.
// WARNING! This library is a HACK and it may stop working
// in future versions of Orbiter API if the library isn't
// maintained anymore. Note however that it's very easy to do so,
// by updating the VesselHooking class
//
// Copyright (C) 2008 Steve "agentgonzo" Arch - VESSEL virtual table hijacking
// Copyright (C) 2012-2013 Szymon "Enjo" Ender - created a framework
//
//                         All rights reserved
//
// HUDDrawer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// HUDDrawer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with HUDDrawer.  If not, see <http://www.gnu.org/licenses/>.
// ==============================================================

#ifndef IMFDDrawsHUD_HPP
#define IMFDDrawsHUD_HPP

#include "Orbitersdk.h"
#include <string>

namespace EnjoLib
{
/// Interface for a class that is supposed to draw on HUD
/**
    Make this a base class of your module or MFD to receive the HUD drawing context.
    For example:
    \code
    #include <Orbitersdk.h>
    #include <EnjoLib/IDrawsHUD.hpp>

    class MyMFD : public MFD2, public EnjoLib::IDrawsHUD
    {
        public:
        // ...
        // your MFD definition
        // ...

        // EnjoLib::IDrawsHUD methods:
        void DrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp);
        bool ShouldDrawHUD() const;
        const char * GetModuleName() const;

        protected:
        private:
    };
    \endcode
    Take note, that the child class doesn't neccessarily need to be an MFD.
*/
class __declspec(dllexport) IDrawsHUD
{
public:
    /// Ctor
    IDrawsHUD();
    /// Dtor
    virtual ~IDrawsHUD();

    /// Should the MFD draw on HUD now?
    /**
        Could return false, for example when the user switched drawing on HUD through an MFD button.
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
    virtual void DrawHUD(int mode, const HUDPAINTSPEC *hps, HDC hDC) {};

    /// Should return unique module's name.
    /**
        It's required so that the module can be disabled globally with HUDDrawer MFD.
    */
    virtual const char * GetModuleName() const = 0;

protected:
private:
};
}

#endif // IMFDDrawsHUD_HPP
