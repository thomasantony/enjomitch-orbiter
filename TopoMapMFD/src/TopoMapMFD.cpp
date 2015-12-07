#define STRICT
#define ORBITER_MODULE

#include "Orbitersdk.h"
#include "TopoMapMFD.h"
#include "MFDButtonPageTopoMFD.h"
#include "PluginTopoMapMFD.h"
#include "TopoMap.h"
#include <Utils/MyDC.h>
#include <cmath>
#include <sstream>

MFDButtonPageTopoMFD gButtonPage;
extern PluginTopoMapMFD * gp_plugin;

using namespace EnjoLib;
using namespace EnjoLib::MFDGoodies;

bool TopoMapMFD::m_rgb = false;
//bool TopoMapMFD::m_rgb = true;

// Constructor
TopoMapMFD::TopoMapMFD (DWORD w, DWORD h, VESSEL *vessel, PluginTopoMapMFD * plugin)
    : MFD2 (w, h, vessel)
    , m_tm(w, h)
    , m_plugin(plugin)
{
    m_plugin->SetTopoMap(&m_tm);
}

// Destructor
TopoMapMFD::~TopoMapMFD ()
{
    // Add MFD cleanup code here
    m_plugin->SetTopoMap(NULL);
}

void TopoMapMFD::DrawEllipse(oapi::Sketchpad * skp, int r, Pens::LineStyle penName)
{
    MFDSetPen(skp, m_pens.GetPen(penName));
    skp->Ellipse(W/2 - r, H/2 - r, W/2 + r, H/2 + r);
}

// Repaint the MFD
bool TopoMapMFD::Update (oapi::Sketchpad * skp)
{
    m_tm.SetRGB(m_rgb);
    m_tm.Draw(skp);
    Title(skp, "Topographic Map MFD");

    DrawEllipse(skp, 2, Pens::Green);
    DrawEllipse(skp, 3, Pens::Red);
    const int x = 2;
    MFDTextOut(skp, x, 20, GREEN, "Refresh lines = %d", m_tm.GetRefreshLines());
    MFDTextOut(skp, x, 30, RED,   "Refresh lines = %d", m_tm.GetRefreshLines());
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
    m_rgb = ! m_rgb;
}
void TopoMapMFD::RefreshIncrement()
{
    m_tm.RefreshIncrement();
}
void TopoMapMFD::RefreshDecrement()
{
    m_tm.RefreshDecrement();
}

// MFD message parser
int TopoMapMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case OAPI_MSG_MFD_OPENED:
        // Our new MFD mode has been selected, so we create the MFD and
        // return a pointer to it.
        return (int) new TopoMapMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam, gp_plugin);
    }
    return 0;
}

void TopoMapMFD::WriteStatus (FILEHANDLE scn) const
{
}

void TopoMapMFD::ReadStatus (FILEHANDLE scn)
{
}

