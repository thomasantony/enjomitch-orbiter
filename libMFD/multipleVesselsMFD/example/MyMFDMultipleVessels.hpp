// ==============================================================
//          ORBITER AUX LIBRARY: Multiple Vessels Support
//             http://sf.net/projects/enjomitchsorbit
//                  Part of the ORBITER SDK
//
// Copyright (C) 2008      Mohd "Computerex" Ali       - original concept
// Copyright (C) 2011      Szymon "Enjo" Ender         - object oriented design,
//														 simplifications, fixes
//                         All rights reserved
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

#ifndef __MyMFDMultipleVessels_H
#define __MyMFDMultipleVessels_H

#include <orbitersdk.h>
#include <EnjoLib/IDrawsHUD.hpp>

// Don't include headers of these classes to speed up compilation.
// Class declarations are enough, since we're using pointers and references only.
class MyMFDData;
class MyPluginMultipleVessels;
class MyMFDButtonsPage;

namespace EnjoLib
{
namespace MFDGoodies
{
	class Sound;
}
}

/// Main MFD class
/**
Main MFD class. Constructed each time the MFD is opened or vessel is changed.
Therefore, if you need to store the values of this and other vessels,
you need to use and external data structure, that stores these values.
Here, it's called MFDData.
*/
class MyMFDMultipleVessels : public MFD2, public EnjoLib::IDrawsHUD
{
public:
    /// Default constructor
    /**
    Constructs the MFD.
    \param w - MFD's width
    \param h - MFD's height
    \param vessel for which the MFD has been opened
    \param PluginMultipleVessels contains configuration and other data neede by the MFD
    */
    MyMFDMultipleVessels (DWORD w, DWORD h, VESSEL * vessel, MyPluginMultipleVessels * pluginMultipleVessels );

    /// Destructor
    ~MyMFDMultipleVessels ();

    /// Returns Sound class
    /** Returns a reference to the Sound class, needed outside the MFD's context
    - for example, in callbacks. */
    EnjoLib::MFDGoodies::Sound & GetSound();

    /// Returns MFDData class
    /** Returns a pointer to the MFDData class, needed outside the MFD's context
     - for example, in callbacks. */
    MyMFDData * GetData();

    /// Creates button label
    /** Refer to Orbiter SDK doc */
    char *ButtonLabel (int bt);

    /// Creates button label
    /** Refer to Orbiter SDK doc */
    int ButtonMenu (const MFDBUTTONMENU **menu) const;

    /// Updates MFD
    /** Refer to Orbiter SDK doc */
    bool Update ( oapi::Sketchpad * skp );

    /// Defines key reaction
    /** Refer to Orbiter SDK doc */
    bool ConsumeKeyBuffered(DWORD key);

    /// Defines mouse reaction
    /** Refer to Orbiter SDK doc */
    bool ConsumeButton (int bt, int event);

    /// Message parser
    /** Refer to Orbiter SDK doc */
    static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

	// IDrawsHUD
	bool ShouldDrawHUD() const;
    void DrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp);
	const char * GetModuleName() const;

    // Callbacks for MFDButtonsPage
    void SwitchButtonsPage();
    void SelectTarget();
    void SwitchAutopilotStandard();
    void SwitchAutopilotSophisticated();
    void SwitchUseSound();
    void SwitchHUD();

private:
    static bool clbkTargetDialog(void *id, char *str, void *usrdata);

    void PrintResults(oapi::Sketchpad * skp) const;

    /// A pointer to the MyMFDData object.
    MyMFDData * m_data;

    /// A reference to the Sound object.
    /** Reference is prefered to pointer, to make sure it's initialised */
    EnjoLib::MFDGoodies::Sound & m_sound;
    MyMFDButtonsPage & m_buttons;
};

#endif // !__MyMFDMultipleVessels_H
