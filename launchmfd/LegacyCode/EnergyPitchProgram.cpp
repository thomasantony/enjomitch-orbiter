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
// Authors - rjcroy, Dave "Daver" Rowbotham, Erik H. "Sputnik" Anderson
//           Szymon "Enjo" Ender - encapsulation and generalization to other planets
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

#include "EnergyPitchProgram.hpp"
#include "MFDDataLaunchMFD.hpp"
#include "Math/GeneralMath.hpp"
#include "Math/SpaceMathBody.hpp"
#include "Utils/VesselCapabilities.hpp"


EnergyPitchProgram::EnergyPitchProgram()
{
    // Energy - Pitch
//    programme.push_back( Point(0.0, 89.0) );
//    programme.push_back( Point(0.4, 80.0) );
//    programme.push_back( Point(0.5, 70.0) );
//    programme.push_back( Point(0.8, 55.0) );
//    programme.push_back( Point(1.2, 48.0) );
//    programme.push_back( Point(1.7, 43.0) );
//    programme.push_back( Point(2.8, 35.0) );
//    programme.push_back( Point(4.0, 24.0) );
//    programme.push_back( Point(7.0, 18.0) );
//    programme.push_back( Point(10.0, 12.0) );
//    programme.push_back( Point(16.0, 7.0) );
//    programme.push_back( Point(22.0, 5.0) );
//    programme.push_back( Point(28.0, 2.5) );
//    programme.push_back( Point(40.0, 0) );

    programme.push_back( Point(0.00, 89.5) );
 //   programme.push_back( Point(0.003, 80.0) );
    programme.push_back( Point(0.04, 35.0) );
    programme.push_back( Point(0.12, 0.001) );

 Flydown = false;
}

EnergyPitchProgram::~EnergyPitchProgram()
{
    //dtor
}

double EnergyPitchProgram::LinearInterpolationProgramme( const std::vector< Point > & programme, double x ) const
{
    if ( programme.empty() )
        return 0;

    if ( x < programme.front().x )
        return programme.front().y;

    if ( x > programme.back().x )
        return programme.back().y;

    for (size_t i = 1; i < programme.size(); ++i)
    {
        if ( programme[i].x < x )
            continue;
        else
            return GeneralMath().LinearInterpol(x, programme[i-1], programme[i]);
    }

    return 0;
}

double EnergyPitchProgram::GetEnergy( const MFDDataLaunchMFD * data )
{
    const double PitchMult = 1;
    VECTOR3 vel = data->GetVel();
    double velMod = length(vel);
    double energy = data->GetCurrentAlt() + velMod*velMod / 2.0;
    energy *= 1e-6;
    energy *= PitchMult;

    return energy;
}

double GetEnergy2( const MFDDataLaunchMFD * data )
{
    SpaceMathBody smb(data->m_bodyPhys.mass, data->m_bodyPhys.radius);
    double velMod = length(data->GetVel());
    double energy = smb.CalcEnergyFromVelRadius( velMod, data->GetCurrentAlt() );
    return energy;
}

double GetEnergyMax( const MFDDataLaunchMFD * data )
{
    SpaceMathBody smb(data->m_bodyPhys.mass, data->m_bodyPhys.radius);
    double energy = smb.CalcEnergyFromRadii( data->ApA, data->PeA );

    return energy;
}

double GetEnergyMin( const MFDDataLaunchMFD * data )
{
    double velAtPole = 0;
    SpaceMathBody smb(data->m_bodyPhys.mass, data->m_bodyPhys.radius);
    double energy = smb.CalcEnergyFromVelRadius( velAtPole, 0 );

    return energy;
}

double GetEnergyRatio(  const MFDDataLaunchMFD * data  )
{
    double energy = GetEnergy2(data);
    double energyMax = GetEnergyMax(data);
    double energyMin = GetEnergyMin(data);
    double energyDistTotal = energyMin - energyMax;
    double energyDistCurrent = energyMin - energy;
    double energyRatio = energyDistCurrent / energyDistTotal;

  //  sprintf( oapiDebugString(), "energyMax = %.02lf, energyMin = %.02lf, energy = %.02lf, ratio = %.04lf", energyMax, energyMin, energy, energyRatio);

    return energyRatio;
}

double EnergyPitchProgram::GetPitch( const MFDDataLaunchMFD * data, double time )
{
    VESSEL * v = data->GetVessel();

    const double r = data->m_bodyPhys.radius + data->GetCurrentAlt();
    const double gravitAccel = data->m_bodyPhys.mass * GGRAV / ( r * r );

    double energy = GetEnergy( data );
    double energyRatio = GetEnergyRatio(data);
    //double Pitch_SP = LinearInterpolationProgramme(programme, energy );
    double Pitch_SP = LinearInterpolationProgramme(programme, energyRatio );

    //sprintf( oapiDebugString(), "energy1 = %.02lf, energy2 = %.02lf, energyRat = %.08lf, ", GetEnergy2(data), GetEnergy3(data), energyRatio);
    //sprintf( oapiDebugString(), "energyRat = %.02lf, pitch = %.02lf", energyRatio, Pitch_SP);

    ELEMENTS elements;
    ORBITPARAM op;
    v->GetElements(NULL, elements, &op, 0,FRAME_EQU);
    double Alt_Apoapsis = op.ApD - data->m_bodyPhys.radius;
    /*		if (Alt_Apoapsis > 170.0) {
    			Pitch_SP = Pitch_SP/4.0;			// Flydown -- we've met our altitude goal
    			if ((GetAOA()*DEG < 2.0))
    				Pitch_SP += 1-1.0*GetAOA()*DEG;
    		}
    */
    double EngineThrust = VesselCapabilities().GetMainThrustParm(v).F;
    //const double ScreenHeight = 125.0;
    const double ScreenHeight = data->PeA;
    if (Alt_Apoapsis < 1.5*ScreenHeight)
        if (v->GetThrusterCount() < 22)			// Special low-power routine
            if (EngineThrust/gravitAccel < 0.6*v->GetMass())
            {
                Pitch_SP += 8.0*((0.6*v->GetMass()) / (EngineThrust/gravitAccel));
                if (Pitch_SP > 40.0) Pitch_SP = 40.0;
            }
    VECTOR3 horiz;
    v->GetHorizonAirspeedVector(horiz);


        VECTOR3 vel = data->GetVel();
        double velMod = length(vel);

        double engineAcc = EngineThrust / v->GetMass();
        double velTangentSquare = horiz.x * horiz.x + horiz.z * horiz.z;
        double velModTangent = sqrt(velTangentSquare);
        double accCentrifugal = velTangentSquare / r;
        double gravityBalance = gravitAccel - accCentrifugal;
        double pitchArg = gravityBalance / engineAcc;
        if ( pitchArg >= 1 )
            pitchArg = 0.99;
        if ( pitchArg <= -1 )
            pitchArg = -0.99;
        double pitch = asin( pitchArg );





        double thrustPitch = engineAcc * sin(v->GetPitch());
        double trueBalance = accCentrifugal + thrustPitch - gravitAccel;

    //sprintf( oapiDebugString(), "horiz.y = %.02lf, pitch = %.02lf, balance = %.02lf, energyRatio = %.02lf", horiz.y, pitch, trueBalance, energyRatio);

    //sprintf( oapiDebugString(), "Alt_Apoapsis = %.02lf, ScreenHeight = %.02lf, horiz.y = %.08lf, ", Alt_Apoapsis, ScreenHeight, horiz.y);
    bool altApoGreaterTgtPeA = Alt_Apoapsis > ScreenHeight;
    bool horizonVelNegative = horiz.y < -10.0;
    if ( altApoGreaterTgtPeA || horizonVelNegative || Flydown )
    {
        Flydown = true;

        if ( horizonVelNegative )
        {
            Pitch_SP = pitch * DEG;
        }
        else
        {
            Pitch_SP = -pitch * DEG;
        }
        const double maxVerticalVel = 50.0;
        if ( fabs ( horiz.y ) < maxVerticalVel )
        {
            Pitch_SP *= fabs ( horiz.y ) / maxVerticalVel;
        }


        //sprintf( oapiDebugString(), "pitch = %.02lf, gravityBalance = %.02lf, acc = %.02lf", pitch, gravityBalance, accCentrifugal);
        //sprintf( oapiDebugString(), "apo = %d, hori = %d, Flydown = %d, ", altApoGreaterTgtPeA, horizonVelNegative, Flydown);
//			Pitch_SP *= 0.25;					// Flydown -- we've met our altitude goal
//        double Pitch_zero = -0.012*horiz.y;
//        //double Aggressive = 0.98-(sqrt(energy)*0.0156);
//        double Aggressive;
////			double Aggressive = 0.85+(energy*0.002);
//        //if (Aggressive > 0.98) Aggressive = 0.98;
//        if (horiz.y > 0.0)
//        {
//            Aggressive = 0.98 - (horiz.y/1000);
//            if (Aggressive < 0.85)
//                Aggressive = 0.85;
//        }
//        else
//        {
//            Aggressive = 0.98 - sqrt(energy) * 0.219;
//        }
//        double RollAtt = 0;
//        if (RollAtt == 0)
//            Pitch_SP += Pitch_zero-Aggressive*v->GetAOA()*DEG;
//        else if (RollAtt == 1)
//            Pitch_SP += Pitch_zero+Aggressive*v->GetAOA()*DEG;
//        if (Alt_Apoapsis < 1.5*ScreenHeight)
//            if (v->GetThrusterCount() < 22)						// Special low-power addition
//                if (EngineThrust/gravitAccel < 0.6*v->GetMass())
//                {
//                    double multiple = 8.0;
//                    multiple /= (horiz.y/25);
//                    Pitch_SP += multiple*((0.6*v->GetMass()) / (EngineThrust/gravitAccel));
//                }
//        if (Pitch_SP > 40.0) Pitch_SP = 40.0;
    }
    //else Pitch_SP += -0.02*horiz.y;
//		else if ((Alt_Apoapsis > 120.0) && (GetAOA()*DEG < 2.0))
//			Pitch_SP += 1-1.0*GetAOA()*DEG;


    /*		if (Alt_Apoapsis < ScreenHeight)
    			if (Pitch_SP < (ScreenHeight - Alt_Apoapsis)/5) {
    				Flydown = true;
    				Pitch_SP = (ScreenHeight - Alt_Apoapsis)/5;
    			}
    */
    double MinPitch = -5; // not 0, because 0 is a special case
    if (Pitch_SP < MinPitch)
    {
        Pitch_SP = MinPitch;

    }
//    if (Pitch_SP == 0)
//    {
//        Pitch_SP = MinPitch;
//
//    }

    double pitchRad = Pitch_SP * RAD;
    return pitchRad;
}


bool EnergyPitchProgram::IsStopCondition( const MFDDataLaunchMFD * data ) const
{
    //double alt = data->GetCurrentAlt();
    //SpaceMathBody smb(data->m_bodyPhys.mass, data->m_bodyPhys.radius);
    // We should have this velocity
    //double velPeriapsis = smb.CalcVelAtRadius(data->ApA, alt, alt);
    //sprintf( oapiDebugString(), "v1 = %.02lf ", velPeriapsis);
    // Get current velocity
    //double velMod = length(data->GetVel());
    //if (GetEnergyRatio(data) >= 1 && (velMod > velPeriapsis))

    ELEMENTS elements;
    ORBITPARAM op;
    data->GetVessel()->GetElements(NULL, elements, &op, 0,FRAME_EQU);
    const double & R = data->m_bodyPhys.radius;

    bool PeAPositive = op.PeD > R;
    bool ApAReached = op.ApD >= data->ApA + R;
    bool energyRatioReached = GetEnergyRatio(data) >= 1;
    if ( energyRatioReached || ( ApAReached && PeAPositive ) )
        return true;
    return false;
}

