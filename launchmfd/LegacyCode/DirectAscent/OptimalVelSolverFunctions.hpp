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

#ifndef OPTIMALVELSOLVERFUNCTIONS_H
#define OPTIMALVELSOLVERFUNCTIONS_H

#include "Math/SimpsonFunctor.hpp"
#include "../../BODYPHYS.h"
#include "SHIP_PARAMS.h"
struct OptimalVelSolverParams;


class CaVertParams
{
public:
    CaVertParams( const BODYPHYS & bodyPhys, const SHIP_PARAMS & shipParam,
                           const OptimalVelSolverParams & optVelPar, double parVelHoriMax );
    double a, g, r, t0, sid, vx, vy, vsatx, vsaty, az, mass, velOrb, initial_rotation_vel_wlat;
};

// vertical acceleration needed to counter gravity
class aVert : public EnjoLib::SimpsonFunctor
{
public:
    aVert( const CaVertParams & params );
    double operator()( double t ) const;
private:
    const CaVertParams & m_par;
};

// vertical acceleration needed to counter gravity
class aVert2 : public EnjoLib::SimpsonFunctor
{
public:
    aVert2( const CaVertParams & params );
    double operator()( double t ) const;
private:
    const CaVertParams & m_par;
};


#endif // OPTIMALVELSOLVERFUNCTIONS_H
