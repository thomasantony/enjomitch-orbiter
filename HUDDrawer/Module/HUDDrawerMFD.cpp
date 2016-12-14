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

#include "HUDDrawerMFD.h"
#include "MFDButtonPageHUDDrawer.h"
#include "../VesselHooking/VesselHooking.hpp"
#include <Utils/MFDTextCalculator.hpp>

using namespace EnjoLib;
using namespace EnjoLib::MFDGoodies;

MFDButtonPageHUDDrawer m_buttons;

HUDDrawerMFD::HUDDrawerMFD (DWORD w, DWORD h, VESSEL * vessel )
: MY_MFD(w, h, vessel)
, iSelected(0)
{
}

HUDDrawerMFD::~HUDDrawerMFD ()
{
}

MFD_RETURN_TYPE HUDDrawerMFD::Update ( MyDC myDC )
{
    Title (myDC, "HUDDrawer MFD");
    MFDTextCalculator c(W, H);
	int ypos = 1;
	const int xpos = 1;
	MFDTextOut(myDC, c.X(xpos), c.Y(ypos++), YELLOW, "Drawers list:")
    const std::vector<VesselHooking::Drawer> & drawers = VesselHooking::GetDrawersList();
    for (unsigned i = 0; i < drawers.size(); ++i)
    {
        const VesselHooking::Drawer & drawer = drawers.at(i);
        const std::string txt = (drawer.isActive ? "+ " : "  ") + drawer.id;
        MFDTextOut(myDC, c.X(xpos), c.Y(ypos++), iSelected == i ? LIGHT_BLUE : YELLOW, txt.c_str());
    }
    return MFD_RETURN_VALUE(true);
}

void HUDDrawerMFD::OnPrevItem()
{
    const std::vector<VesselHooking::Drawer> & drawers = VesselHooking::GetDrawersList();
    if (drawers.empty())
        return;
    if ( --iSelected < 0 )
        iSelected = drawers.size() - 1;
}

void HUDDrawerMFD::OnNextItem()
{
    const std::vector<VesselHooking::Drawer> & drawers = VesselHooking::GetDrawersList();
    if (drawers.empty())
        return;
    iSelected = ++iSelected % drawers.size();
}

void HUDDrawerMFD::OnModItem()
{
    const std::vector<VesselHooking::Drawer> & drawers = VesselHooking::GetDrawersList();
    if (drawers.empty())
        return;
    if (iSelected < (int)drawers.size())
    {
        VesselHooking::Drawer drawer = drawers.at(iSelected);
        drawer.isActive = ! drawer.isActive;
        VesselHooking::UpdateDrawer( drawer );
    }
}

void HUDDrawerMFD::OnEnableAll()
{
    VesselHooking::EnableAllDrawers();
}

void HUDDrawerMFD::OnDisableAll()
{
    VesselHooking::DisableAllDrawers();
}

char * HUDDrawerMFD::ButtonLabel (int bt)
{
    return m_buttons.ButtonLabel(bt);
}

int HUDDrawerMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
    return m_buttons.ButtonMenu(menu);
}

bool HUDDrawerMFD::ConsumeKeyBuffered(DWORD key)
{
    return m_buttons.ConsumeKeyBuffered(this, key);
}

bool HUDDrawerMFD::ConsumeButton (int bt, int event)
{
    return m_buttons.ConsumeButton(this, bt, event);
}

int HUDDrawerMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case OAPI_MSG_MFD_OPENED:
        // Our new MFD mode has been selected, so we create the MFD and
        // return a pointer to it.
		return (int) new HUDDrawerMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam);
    }
    return 0;
}
