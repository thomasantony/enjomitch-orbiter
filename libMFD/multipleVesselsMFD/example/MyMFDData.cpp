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

#include "MyMFDData.hpp"

MyMFDData::MyMFDData( VESSEL * vessel )
    : MFDData( vessel )
    , m_drawHUD( true )
    , m_isTgtSet( false )
    , autopilotType( AP_NONE )
{
    Update();
}

MyMFDData::~MyMFDData()
{

}

void MyMFDData::Update()
{
    // Check velocity and position
    UpdatePositionVelocity();
    // check if a planet has changed. If so, reset some parameters
    CheckPlanetChange();
    // get realtime target's orbital inclination and other parameters defined in TGTPARAM
    UpdateTargetParams();
}

void MyMFDData::UpdatePositionVelocity()
{
    GetVessel()->GetEquPos(m_longitude, m_latitude, m_radius);
}

void MyMFDData::UpdateTargetParams()
{
    m_tgtParam = CheckTgtPosition();
}

TGTPARAM & MyMFDData::GetTargetParams()
{
    return m_tgtParam;
}


TGTPARAM MyMFDData::CheckTgtPosition()
{
    TGTPARAM tgtParamLocal;
    if (hRef && m_isTgtSet)
    {
        OBJHANDLE hTarget = oapiGetVesselByName(strTarget);
        if (hTarget != NULL  && hRef != NULL)
        {
            VESSEL * target = oapiGetVesselInterface(hTarget);
            if (target!=NULL)
            {
                ELEMENTS e;
                ORBITPARAM op;
                target->GetElements(NULL, e, &op, 0,FRAME_EQU);
                tgtParamLocal.incl = e.i;
            }
        }
    } // if tgt_set

    return tgtParamLocal;
}

BODYPHYS MyMFDData::GetPlanetParameters (OBJHANDLE hRef) const
{
    BODYPHYS temp;
    if (hRef != NULL)
    {
        temp.radius = oapiGetSize(hRef);
        temp.mass = oapiGetMass(hRef);
        temp.sidDay = oapiGetPlanetPeriod(hRef);
    }
    return temp;
}

// check if a planet has changed. If so, reset some parameters
void MyMFDData::CheckPlanetChange()
{
    OBJHANDLE currentRef = GetVessel()->GetSurfaceRef();
    if ( hRef == currentRef )
        return;

    hRef = currentRef;
    m_bodyPhys = GetPlanetParameters (hRef);
}

AutopilotType MyMFDData::GetAutopilotType() const
{
    return autopilotType;
}

void MyMFDData::SwitchAutopilot( AutopilotType type )
{
    if (autopilotType != AP_NONE )
        // Deactivaqte, no matter what was on
        autopilotType = AP_NONE;
    else
        autopilotType = type;

    // Some autopi;
    GetVessel()->DeactivateNavmode( NAVMODE_PROGRADE );
    GetVessel()->DeactivateNavmode( NAVMODE_RETROGRADE );
    GetVessel()->DeactivateNavmode( NAVMODE_NORMAL );
    GetVessel()->DeactivateNavmode( NAVMODE_ANTINORMAL );
    if ( autopilotType != AP_SOPHISTICATED )
        GetVessel()->DeactivateNavmode( NAVMODE_HLEVEL ); // you may want this in DA

    if ( autopilotType == AP_NONE )
    {
        // Disable automatic control
        GetVessel()->SetAttitudeRotLevel (0, 0);
        GetVessel()->SetAttitudeRotLevel (1, 0);
        GetVessel()->SetAttitudeRotLevel (2, 0);

        GetVessel()->SetControlSurfaceLevel( AIRCTRL_RUDDERTRIM, 0 );
        GetVessel()->SetControlSurfaceLevel( AIRCTRL_ELEVATORTRIM, 0 );

        GetVessel()->ActivateNavmode( NAVMODE_KILLROT );
    }
    else
    {
        // Let the AP take control and don's stop it
        GetVessel()->DeactivateNavmode( NAVMODE_KILLROT );
    }
}

void MyMFDData::MECO()
{
    THGROUP_HANDLE h = GetVessel()->GetThrusterGroupHandle( THGROUP_MAIN );
    if ( h != NULL )
    {
        GetVessel()->SetThrusterGroupLevel( h, 0 );
    }
}

void MyMFDData::MainEngineOn()
{
    THGROUP_HANDLE h = GetVessel()->GetThrusterGroupHandle( THGROUP_MAIN );
    if ( h != NULL )
    {
        GetVessel()->SetThrusterGroupLevel( h, 1 );
    }
}

void MyMFDData::HoverEngineOn()
{
    THGROUP_HANDLE h = GetVessel()->GetThrusterGroupHandle( THGROUP_HOVER );
    if ( h != NULL )
    {
        GetVessel()->SetThrusterGroupLevel( h, 1 );
    }
}
