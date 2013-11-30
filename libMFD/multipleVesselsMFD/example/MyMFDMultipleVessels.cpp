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

#include "MyPluginMultipleVessels.hpp"
#include "MyMFDMultipleVessels.hpp"
#include "../../MFDSound++/SoundMap.hpp"
#include "Sound/SoundSampleIDEnum.hpp"
#include "MyMFDData.hpp"
#include "orbitersdk.h"

using namespace EnjoLib::MFDGoodies;


// ==============================================================
// Global variables
int g_MFDmode; // identifier for new MFD mode

MyPluginMultipleVessels * pMyPluginMultipleVessels;

// ==============================================================
// API interface

// Called on module init
DLLCLBK void InitModule (HINSTANCE hDLL)
{
    static char *name = "MyMFDMultipleVessels";   // MFD mode name
    MFDMODESPEC spec;
    // Set MFDMODESPEC's fields
    spec.name = name;
    spec.key = OAPI_KEY_T;                // MFD mode selection key
    spec.msgproc = MyMFDMultipleVessels::MsgProc;  // MFD mode callback function

    // Register the new MFD mode with Orbiter
    g_MFDmode = oapiRegisterMFDMode (spec);
    pMyPluginMultipleVessels = new MyPluginMultipleVessels(hDLL);
    oapiRegisterModule (pMyPluginMultipleVessels);
}

// Called on module exit
DLLCLBK void ExitModule (HINSTANCE hDLL)
{
    // Unregister the custom MFD mode when the module is unloaded
    oapiUnregisterMFDMode (g_MFDmode);
}

// ==============================================================
// MFD class implementation

// Constructor
MyMFDMultipleVessels::MyMFDMultipleVessels (DWORD w, DWORD h, VESSEL *vessel, MyPluginMultipleVessels * pluginMultipleVessels ) :
    MFD2 (w, h, vessel)
// Initialisation list
// init m_data with PluginMultipleVessels's return value, depending on the vessel pointer
// If dynamic cast fails, the m_data member becomes NULL
    , m_data(dynamic_cast<MyMFDData *>(pluginMultipleVessels->AssociateMFDData(vessel)))
    , m_sound(pluginMultipleVessels->GetSound())
    , m_buttons(pluginMultipleVessels->GetButtons())
{

    if ( m_data == NULL ) // Programming error
        sprintf_s(oapiDebugString(), 512, "m_data pointer type is not compatible with the pointer that was being assigned to it in Ctor");
}

// Destructor
MyMFDMultipleVessels::~MyMFDMultipleVessels ()
{
    // Add MFD cleanup code here
}

// Return button labels
char *MyMFDMultipleVessels::ButtonLabel (int bt)
{
    return m_buttons.ButtonLabel(bt);
}

// Return button menus
int MyMFDMultipleVessels::ButtonMenu (const MFDBUTTONMENU **menu) const
{
    return m_buttons.ButtonMenu(menu);
}

// Repaint the MFD
bool MyMFDMultipleVessels::Update ( oapi::Sketchpad * skp )
{
    if ( m_data == NULL )
        return false;
    // Sound can be connected only within Update
    m_sound.Connect( "MyMFDMultipleVessels_Enjo, lol");
    // Update all ship's variables
    m_data->Update();

    // Draws the MFD title
    Title (skp, "My Multiple Vessels MFD");

    PrintResults(skp);

    return true;
}

bool MyMFDMultipleVessels::ConsumeButton (int bt, int event)
{
    return m_buttons.ConsumeButton(this, bt, event);
}

bool MyMFDMultipleVessels::ConsumeKeyBuffered(DWORD key)
{
    return m_buttons.ConsumeKeyBuffered(this, key);
}

void MyMFDMultipleVessels::SelectTarget()
{
    oapiOpenInputBox("Target", clbkTargetDialog, "", 20, this);
}
void MyMFDMultipleVessels::SwitchAutopilotStandard()
{
    m_data->SwitchAutopilot( AP_STANDARD );
}
void MyMFDMultipleVessels::SwitchAutopilotSophisticated()
{
    m_data->SwitchAutopilot( AP_SOPHISTICATED );
}
void MyMFDMultipleVessels::SwitchUseSound()
{
    m_sound.SwitchUseSound();
}
void MyMFDMultipleVessels::SwitchButtonsPage()
{
    m_buttons.SwitchPage(this);
}
void MyMFDMultipleVessels::SwitchHUD()
{
   m_data->m_drawHUD = ! m_data->m_drawHUD;
}
// MFD message parser
int MyMFDMultipleVessels::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case OAPI_MSG_MFD_OPENED:
        // Our new MFD mode has been selected, so we create the MFD and
        // return a pointer to it.
		return (int) new MyMFDMultipleVessels (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam, pMyPluginMultipleVessels);
    }
    return 0;
}

void MyMFDMultipleVessels::PrintResults( oapi::Sketchpad * skp)  const
{
    const int MFD_LINE_NR = 24;
    const int MFD_CHAR_LEN = 35;
    char cbuf[BUFLEN];
    int len;
    int line = 2; // start point of the below strings
    int linespacing = H/MFD_LINE_NR;
    int charwidth = W/MFD_CHAR_LEN;

    if ( m_data->m_isTgtSet )
    {
        len = strlen(m_data->strTarget);
        skp->Text(2 * charwidth, (line++)* linespacing, m_data->strTarget, len);
    }
    len = sprintf_s(cbuf, BUFLEN, "Longitude: %f", m_data->m_longitude);
    skp->Text(2 * charwidth, (line++)* linespacing, cbuf, len);
    len = sprintf_s(cbuf, BUFLEN, "Latitude: %f", m_data->m_latitude);
    skp->Text(2 * charwidth, (line++)* linespacing, cbuf, len);


    switch (m_data->GetAutopilotType())
    {
    case AP_STANDARD:
    {
        const char * apName = "Standard autopilot";
        skp->Text(2 * charwidth, (line++)* linespacing, apName, strlen(apName));
    }
    break;
    case AP_SOPHISTICATED:
    {
        const char * apName = "Sophisticated autopilot";
        skp->Text(2 * charwidth, (line++)* linespacing, apName, strlen(apName));
    }
    case AP_NONE:
        break;
    }
}

Sound & MyMFDMultipleVessels::GetSound()
{
    return m_sound;
}

MyMFDData * MyMFDMultipleVessels::GetData()
{
    return m_data;
}


// Callback function for target input dialog
bool MyMFDMultipleVessels::clbkTargetDialog(void *id, char *str, void *usrdata)
{
    MyMFDMultipleVessels* cMyMFDMultipleVessels = (MyMFDMultipleVessels*)usrdata;
    OBJHANDLE hVessel = oapiGetVesselByName(str);
    if (hVessel != NULL)
    {
        VESSEL * v = oapiGetVesselInterface(hVessel);
        if (v == NULL)
        {
            return false;
        }
        ELEMENTS e;
        ORBITPARAM op;
        v->GetElements(NULL, e, &op, 0,FRAME_EQU);
        if (abs(cMyMFDMultipleVessels->GetData()->m_latitude * DEG) < (e.i * DEG))
        {
            cMyMFDMultipleVessels->GetData()->m_isTgtSet = true;
            cMyMFDMultipleVessels->GetData()->GetTargetParams().incl = e.i;
            //data->tgtParam.incl = e.i;
            //cMyMFDMultipleVessels->GetData()->refAlt = e.a - cMyMFDMultipleVessels->GetData()->bodyPhys.radius; // no no
            oapiGetObjectName(hVessel, cMyMFDMultipleVessels->GetData()->strTarget, BUFLEN);
            //oapiGetObjectName(objTarget, data->strTarget, 34);
            cMyMFDMultipleVessels->GetSound().PlaySound(TARGET_SELECTED);
        }
        else cMyMFDMultipleVessels->GetData()->m_isTgtSet = false;
        // FIXME - the same for GBody
    }
    return(true);
}

bool MyMFDMultipleVessels::ShouldDrawHUD() const
{
	return m_data->m_drawHUD;
}

void MyMFDMultipleVessels::DrawHUD(int mode, const HUDPAINTSPEC *hps, oapi::Sketchpad * skp)
{
	char buf[1024];
	sprintf_s(buf, 1024, "Hy. I be text from ur MFD");
	skp->Text(500, 500, buf, (int) strlen(buf));
}

const char * MyMFDMultipleVessels::GetModuleName() const
{
	return "My MFD MultipleVessels";
}
