// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#define STRICT
#define ORBITER_MODULE

#include "Orbitersdk.h"
#include "TopoMapMFD.h"
#include "MFDButtonPageTopoMFD.h"
#include "PluginTopoMapMFD.h"
#include "TopoMap.h"
#include <Utils/MyDC.h>
#include <Utils/MFDTextCalculator.hpp>
#include <cmath>
#include <sstream>
//#include "OGCImy.h"

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
    MFDTextCalculator mcalc(W, H);
    const int x = 1;
    int y = 2;
    std::ostringstream ossLines; ossLines << "Refresh lines = " << m_tm.GetRefreshLines();
    std::ostringstream ossZoom;  ossZoom  << "Zoom level = " << int(m_tm.GetZoom()) << " " << (m_tm.IsZoomAuto() ? "Auto" : "");
    if (m_rgb)
    {
        MFDTextOut(skp, mcalc.X(x), mcalc.Y(y++), BLUE,  ossLines.str().c_str());
        MFDTextOut(skp, mcalc.X(x), mcalc.Y(y++), RED,   ossLines.str().c_str());
        MFDTextOut(skp, mcalc.X(x), mcalc.Y(y++), GREEN, ossLines.str().c_str());
        y++;
        MFDTextOut(skp, mcalc.X(x), mcalc.Y(y++), BLUE,  ossZoom.str().c_str());
        MFDTextOut(skp, mcalc.X(x), mcalc.Y(y++), RED,   ossZoom.str().c_str());
        MFDTextOut(skp, mcalc.X(x), mcalc.Y(y++), GREEN, ossZoom.str().c_str());

    }
    else
    {
        MFDTextOut(skp, mcalc.X(x), mcalc.Y(y++), BLUE,  ossLines.str().c_str());
        y+=3;
        MFDTextOut(skp, mcalc.X(x), mcalc.Y(y++), BLUE,  ossZoom.str().c_str());
    }


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
void TopoMapMFD::ZoomAutoSwitch()
{
    m_tm.ZoomAutoSwitch();
}
void TopoMapMFD::ZoomMaximal()
{
    m_tm.ZoomMaximal();
}
void TopoMapMFD::ZoomIn()
{
    m_tm.ZoomIn();
}
void TopoMapMFD::ZoomOut()
{
    m_tm.ZoomOut();
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

