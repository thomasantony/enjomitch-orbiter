// ==============================================================
//                 ORBITER MODULE: LaunchMFD
//                  Part of the ORBITER SDK
//
// Copyright (C) 2004      rjcroy                      - robust time based pitch autopilot (borrowed code)
// Copyright (C) 2004      Dave "Daver" Rowbotham      - conversion of rjcroy's autopolot to C++ (borrowed code)
// Copyright (C) 2004      Erik H. "Sputnik" Anderson  - conversion of the autopilot to energy based (borrowed code)
// Copyright (C) 2007      "Vanguard"                  - dressing up azimuth calcualtions into an MFD (author)
// Copyright (C) 2007      Pawel "She'da'Lier" Stiasny - yaw error visual representation (contributor)
// Copyright (C) 2008      Mohd "Computerex" Ali       - borrowed his code (multiple vessels support) (borrowed code)
// Copyright (C) 2008      Chris "Kwan" Jeppesen       - borrowed his code (peg guidance) (borrowed code)
// Copyright (C) 2008      Steve "agentgonzo" Arch     - peg integration, offplane correction, compass, hud display (co-developer)
// Copyright (C) 2007-2012 Szymon "Enjo" Ender         - everything else ;> (author and maintainer)
//                         All rights reserved
//
// Authors - Szymon "Enjo" Ender
//
// This module calculates the appropriate launch azimuth given
// desired orbital inclination and desired orbit altitude. This
// MFD takes the planets rotation into account, which provides a
// much more accurate azimuth. The calculations are performed
// 'on the fly' (technically and methaphorically), meaning that
// you get info about necessary course corrections.
//
// This file is part of LaunchMFD.
//
// LaunchMFD is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaunchMFD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaunchMFD.  If not, see <http://www.gnu.org/licenses/>.
// ==============================================================

#include "AutopilotBase.hpp"
#include "../MFDDataLaunchMFD.hpp"
#include "../../lib/Systems/Point.hpp"
#include "../../lib/Math/AzimuthMath.hpp"

using namespace EnjoLib;

AutopilotBase::AutopilotBase()
{
}

bool AutopilotBase::NeedPitchMain( MFDDataLaunchMFD * data ) const
{
    if ( ! data->drawPitchError )
        return false;
    else
        return NeedPitch( data );
}

bool AutopilotBase::Guide( MFDDataLaunchMFD * data, double dt ) const
{
    data->ReactOnShipStatus();
    if ( !data->launched )
        return false;

    data->SetPEGApses();
    data->Update();


    double targetPitch;
    if ( NeedPitchMain( data ) )
        targetPitch = data->GetTargetPitch();
    else
        targetPitch = data->GetVessel()->GetPitch();

    if ( NeedMECO( data ) )
    {
        if ( data->IsStopConditionPitch() )
		{
            data->SwitchAutopilot( AP_NONE );
            data->MECO();
            return false;
		}
    }

    if ( TerminationRequested( data ) )
    {
        data->SwitchAutopilot( AP_NONE );
        data->MECO();
        return false;
    }

    data->peg.GetTargetPitch(); // must be called anyway to get TMECO later

    VECTOR3 fdTargetVector;

    double azimuth;
    if ( NeedYaw( data ) )
        azimuth = data->GetAzimuth();
    else
        // Artificially get straight line azimuth
        azimuth = data->GetYaw();

    if (data->GetTgtParam().azFlag == BOTH_AZIMUTHS)
    {
        const double heading = data->GetYaw() * DEG;
        if ((heading>90) && (heading<270))	// shouthern
            fdTargetVector = data->flightDirector.GetTargetVector(data->GetVessel(), targetPitch, PI - azimuth);
        else // northern
            fdTargetVector = data->flightDirector.GetTargetVector(data->GetVessel(), targetPitch, azimuth);
    }
    else
    {
        if(data->GetTgtParam().azFlag == NORTHERN_AZIMUTH )
            fdTargetVector = data->flightDirector.GetTargetVector(data->GetVessel(), targetPitch, azimuth);
        else
            fdTargetVector = data->flightDirector.GetTargetVector(data->GetVessel(), targetPitch, PI - azimuth);
    }
    Point targetVector2D(fdTargetVector.x / PI, fdTargetVector.y / PI);
    //sprintf(oapiDebugString(),  "x = %0.2lf, y = %0.2lf, z = %0.2lf",currMaxAngAcc.x, currMaxAngAcc.y, currMaxAngAcc.z);

    VECTOR3 accRatio = data->m_pidAP.GetVesselAngularAccelerationRatio(data->GetVessel());

    //sprintf(oapiDebugString(),  "x = %0.2lf, y = %0.2lf, z = %0.2lf",accRatio.x, accRatio.y, accRatio.z);



    if ( data->IsInAtmosphere() ) // still in dense atmosphere
    {
        AtmoAutopilot( data, dt, targetVector2D, accRatio );
    }
    else // in space
    {
        //data->m_pidAPSpaceY.Update( targetVector2D.y, dt );
        SpaceAutopilot( data, dt, targetVector2D, accRatio ) ;
    }

    data->m_pidAP.m_prevNeededPitch = NeedPitchMain( data );
    return true;
}

// support for upside-down flying craft like the Shuttle
double AutopilotBase::UpdateBankUpsideDown( MFDDataLaunchMFD * data, double inputBank ) const
{
    if ( ! data->m_pidAP.IsRollNeeded() )
        return inputBank; // let the AP decide
    //else if ( data->GetCurrentAlt() < data->m_pidAP.GetRollAlt() )
    else if ( data->GetVessel()->GetPitch() * DEG > 88.5 )
        return 0; // Don't bank at all until "safe" pitch reached. Otherwise the roll may be incorrect
    else if ( data->GetCurrentAlt() < data->m_pidAP.GetRollAlt() )
        return 0; // User wanted us to roll a bit higher
    else
    {
        if ( data->m_pidAP.IsUpsideDown() )
        { // convert 0*->180*
            if ( inputBank < 0 )
            // input bank is scaled down to PI
                inputBank += 1; // Adding PI
            else
                inputBank -= 1;
        } // Else roll still needed, but not upside down
        return inputBank;
    }
}

void AutopilotBase::AtmoAutopilot( MFDDataLaunchMFD * data, double dt, const Point & targetVector2D, const VECTOR3 & accRatio ) const
{
    const double ratioAtmKd = CalculateRatioKdForAtmAP( data );
    const double finalKd = ratioAtmKd * data->m_pidAP.GetReferenceKdForAtmo();
    const double atmoContinuouSwitch = data->m_pidAP.GetAtmXYContinuousControlSwitch();
    double x;
    if (fabs(targetVector2D.x) > atmoContinuouSwitch)
        // Apply full power if beyond a given range
        x = targetVector2D.x > 0 ? 1 : -1;
    else
    {
        // Else use continuous control
        data->m_pidAP.m_pidAPAtmX.SetKd(finalKd);
        x = data->m_pidAP.m_pidAPAtmX.Update( targetVector2D.x / atmoContinuouSwitch, dt );
    }

    data->GetVessel()->SetAttitudeRotLevel( 1, -x );
    if ( data->GetShipVariables().oldAtmosphericModel )
        data->GetVessel()->SetControlSurfaceLevel( AIRCTRL_RUDDER, x/3.0 );
    else
        data->GetVessel()->SetControlSurfaceLevel( AIRCTRL_RUDDERTRIM, x );

    const double inputBankWingsLevel = data->GetVessel()->GetBank() / PI; // 0 is wings level
    const double inputBankWingsLevelUpd = UpdateBankUpsideDown( data, inputBankWingsLevel );
    double b;
    if ( NeedPitchMain( data ) )
    {
        if (targetVector2D.len() < data->m_pidAP.GetAtmBankTargetWindsLevelSwitch() )
        {
            //sprintf(oapiDebugString(),  "b1 = %0.2lf, b2 = %0.2lf", inputBankWingsLevel * DEG, inputBankWingsLevelUpd * DEG);
            b = data->m_pidAP.m_pidAPAtmBankWingsLevel.Update( inputBankWingsLevelUpd, dt );
        }
        else
        {
            const double inputBank = atan2( targetVector2D.x , targetVector2D.y ) / PI;
            const double inputBankUpd = UpdateBankUpsideDown( data, inputBank );
            b = data->m_pidAP.m_pidAPAtmBankTarget.Update( inputBankUpd, dt );
        }

        double y;
        if (fabs(targetVector2D.y) > atmoContinuouSwitch)
            y = targetVector2D.y > 0 ? 1 : -1;
        else
        {
            data->m_pidAP.m_pidAPAtmY.SetKd(finalKd);
            y = data->m_pidAP.m_pidAPAtmY.Update( targetVector2D.y / atmoContinuouSwitch, dt );
        }

        //sprintf(oapiDebugString(),  "atm x = %0.2lf,y = %0.2lf,b = %0.2lf, test = %0.2lf, test2 = %0.2lf",x,y,b, targetVector2D.len(), fabs(targetVector2D.x));

        data->GetVessel()->SetAttitudeRotLevel( 0, y );
        if ( data->GetShipVariables().oldAtmosphericModel )
            data->GetVessel()->SetControlSurfaceLevel( AIRCTRL_ELEVATOR, y/3.0 );
        else
            data->GetVessel()->SetControlSurfaceLevel( AIRCTRL_ELEVATORTRIM, y );

        if ( NeedPullUp( data ) )
            data->GetVessel()->SetControlSurfaceLevel( AIRCTRL_ELEVATOR, y );

    }
    else
    {
        double cuntinuousVar = data->m_pidAP.GetAtmXYContinuousControlSwitch(); // data->m_pidAP.GetAtmBankTargetWindsLevelSwitch()
        const double maxAngle = 45 * RAD / PI;
        double targetBank;
        const double currentBank = (data->GetVessel()->GetBank() ) / PI;
        if (fabs(targetVector2D.x) < cuntinuousVar )
            targetBank = - maxAngle * targetVector2D.x / cuntinuousVar;
        else
            targetBank = targetVector2D.x > 0 ? -maxAngle : maxAngle;

        const double error = currentBank - targetBank;
        b = data->m_pidAP.m_pidAPAtmBankTarget.Update( error, dt );
//        sprintf(oapiDebugString(),  "atm x = %0.2lf, b = %0.2lf",x,b);

        if ( ! NeedPitchMain(data) && data->m_pidAP.m_prevNeededPitch )
        {
            data->GetVessel()->SetAttitudeRotLevel( 0, 0 );
            if ( data->GetShipVariables().oldAtmosphericModel )
                data->GetVessel()->SetControlSurfaceLevel( AIRCTRL_ELEVATOR, 0 );
            else
                data->GetVessel()->SetControlSurfaceLevel( AIRCTRL_ELEVATORTRIM, 0 );
        }
    }
    //sprintf_s(oapiDebugString(), 512, "ratioKD = %0.4lf, kd x = %0.4lf, y = %0.4lf", ratioAtmKd, data->m_pidAP.m_pidAPAtmX.GetKd(), data->m_pidAP.m_pidAPAtmY.GetKd());
    data->GetVessel()->SetAttitudeRotLevel( 2, b );
    data->GetVessel()->SetControlSurfaceLevel( AIRCTRL_AILERON, b );
}

void AutopilotBase::SpaceAutopilot( MFDDataLaunchMFD * data, double dt, const Point & targetVector2D, const VECTOR3 & accRatio ) const
{
    const double x = accRatio.x * data->m_pidAP.m_pidAPSpaceX.Update( targetVector2D.x, dt );
    data->GetVessel()->SetAttitudeRotLevel( 1, -x );

    const double inputBank = data->GetVessel()->GetBank() / PI;
    const double inputBankUpd = UpdateBankUpsideDown( data, inputBank );
    const double b = accRatio.z * data->m_pidAP.m_pidAPSpaceBank.Update( inputBankUpd, dt );
    data->GetVessel()->SetAttitudeRotLevel( 2, b );

    if ( NeedPitchMain ( data ) )
    {
        const double y = accRatio.y * data->m_pidAP.m_pidAPSpaceY.Update( targetVector2D.y, dt );
        data->GetVessel()->SetAttitudeRotLevel( 0, y );
        //sprintf(oapiDebugString(),  "spc x = %0.2lf,y = %0.2lf,b = %0.2lf",x,y,b);
    }
    else
    {
        //sprintf(oapiDebugString(),  "spc x = %0.2lf ,b = %0.2lf",x,b);
    }
}

double AutopilotBase::CalculateRatioKdForAtmAP( const MFDDataLaunchMFD * data ) const
{
    const double maxAtmPressure = data->m_bodyPhys.pressure0;
    if ( maxAtmPressure == 0 )
        return 1;
    //double currentAtmPressure = data->GetVessel()->GetDynPressure();
    double currentAtmPressure = data->GetVessel()->GetAtmPressure(); // more predictable

    // evaluates to 0 at sea level (no Kd needed) to 1 in space (full value of Kd needed)
    double ratio = ( maxAtmPressure - currentAtmPressure ) / maxAtmPressure;

    if (ratio < 0)
        ratio = 0;
    return ratio;
}
