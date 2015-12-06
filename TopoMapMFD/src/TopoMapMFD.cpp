#define STRICT
#define ORBITER_MODULE

#include "Orbitersdk.h"
#include "TopoMapMFD.h"
#include "MFDButtonPageTopoMFD.h"
#include "TopoMap.h"
#include <cmath>
#include <sstream>

MFDButtonPageTopoMFD gButtonPage;

using namespace EnjoLib;
using namespace EnjoLib::MFDGoodies;
// Constructor
TopoMapMFD::TopoMapMFD (DWORD w, DWORD h, VESSEL *vessel)
    : MFD2 (w, h, vessel)
{
    m_grey = true;
}

// Destructor
TopoMapMFD::~TopoMapMFD ()
{
    // Add MFD cleanup code here
}

// Repaint the MFD
bool TopoMapMFD::Update (oapi::Sketchpad * skp)
{
    m_tm.UpdateMap(W, H);
    m_tm.Draw(W, H, skp);
    return true;
}

// Return button labels
char * TopoMapMFD::ButtonLabel (int bt)
{
    return gButtonPage.ButtonLabel(bt);
}

// Return button menus
int TopoMapMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
    return gButtonPage.ButtonMenu( menu );
}

bool TopoMapMFD::ConsumeButton (int bt, int event)
{
    return gButtonPage.ConsumeButton(this, bt, event);
}

bool TopoMapMFD::ConsumeKeyBuffered(DWORD key)
{
    return gButtonPage.ConsumeKeyBuffered(this, key);
}

bool TopoMapMFD::ConsumeKeyImmediate( char * kstate )
{
    return gButtonPage.ConsumeKeyImmediate(this, kstate);
}

// Event handlers
void TopoMapMFD::SwitchButtonsPage()
{
    gButtonPage.SwitchPage(this);
}

void TopoMapMFD::DoNothing()
{
}
void TopoMapMFD::SwitchRGBGrey()
{
    m_grey = ! m_grey;
}

// MFD message parser
int TopoMapMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case OAPI_MSG_MFD_OPENED:
        // Our new MFD mode has been selected, so we create the MFD and
        // return a pointer to it.
        return (int) new TopoMapMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam);
    }
    return 0;
}

void TopoMapMFD::WriteStatus (FILEHANDLE scn) const
{
}

void TopoMapMFD::ReadStatus (FILEHANDLE scn)
{
}

