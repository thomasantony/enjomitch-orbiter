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

#ifndef MYMFDDATA_HPP_INCLUDED
#define MYMFDDATA_HPP_INCLUDED

#include "Autopilot/AutopilotType.hpp"
#include "../MFDData.hpp"
#include "Types.hpp"
#include "Globals.hpp"

#include <orbitersdk.h>


class MyMFDData : public EnjoLib::MFDGoodies::MFDData
{
public:
    MyMFDData( VESSEL * vessel );
    ~MyMFDData();
    void Update();
    void SwitchAutopilot( AutopilotType type );
    AutopilotType GetAutopilotType() const;
    void MECO();
    void MainEngineOn();
    void HoverEngineOn();
    TGTPARAM & GetTargetParams();

    OBJHANDLE hRef;
    TGTPARAM m_tgtParam;
    BODYPHYS m_bodyPhys;
    char strTarget[BUFLEN];
    double m_longitude, m_latitude, m_radius;
    bool m_isTgtSet;
    bool m_drawHUD;

private:
    BODYPHYS GetPlanetParameters (OBJHANDLE hRef) const;
    TGTPARAM CheckTgtPosition();
    void UpdatePositionVelocity();
    void UpdateTargetParams();
    void CheckPlanetChange();


    AutopilotType autopilotType;


};

#endif // MYMFDDATA_HPP_INCLUDED
