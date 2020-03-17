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

#include "IMFDDrawsHUD.hpp"
#include "MFDHUDDrawer.hpp"

using namespace EnjoLib::MFDGoodies;

IMFDDrawsHUD::IMFDDrawsHUD()
{
    MFDHUDDrawer::OnMFDConstruction(this);
}

IMFDDrawsHUD::~IMFDDrawsHUD()
{
    MFDHUDDrawer::OnMFDDestruction();
}

void IMFDDrawsHUD::DrawHUD(int mode, const HUDPAINTSPEC *hps, HDC hDC)
{
}
