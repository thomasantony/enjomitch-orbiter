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

#define _USE_MATH_DEFINES
#include <cmath>

#include "OptimalVelSolverFunctions.hpp"
#include "OptimalVelSolverParams.hpp"
#include <Math/Constants.hpp>
#include <Systems/Point.hpp>
using namespace EnjoLib;

CaVertParams::CaVertParams( const BODYPHYS & bodyPhys, const SHIP_PARAMS & shipParam,
                            const OptimalVelSolverParams & optVelPar, double parVelHoriMax )
{
    //a = acc; // m/s^2
    a = shipParam.thrustHori / shipParam.massFull; // m/s^2

    vx = vy = 0;
    vsatx = 0;
    vsaty = 0;
    az = 0;
    t0 = 0;

    r = optVelPar.target_radius;
    sid = bodyPhys.sidDay;
    mass = bodyPhys.mass;

    double mi = GGRAV_*mass;  //gravitational parameter
    velOrb = sqrt(mi/r); // target's northern velocity

    g = mi / (r * r);
    initial_rotation_vel_wlat = bodyPhys.radius * (2.0*M_PI/sid) * cos(optVelPar.latStart);
}

aVert::aVert( const CaVertParams & params )
    : m_par(params)
{
}

aVert2::aVert2( const CaVertParams & params )
    : m_par(params)
{
}

double aVert::operator()( double t ) const
{
    t = t - m_par.t0;
    double v = t*m_par.a + m_par.initial_rotation_vel_wlat;
    double ret = m_par.g - ( v*v ) / m_par.r;
    return ret;
}


// vertical acceleration needed to counter gravity when reaching final velocity
double aVert2::operator()( double t ) const
{
    t = t - m_par.t0;

    double vnx = m_par.vx + m_par.a * t * sin(m_par.az);
    double vny = m_par.vy + m_par.a * t * cos(m_par.az);
    double vn_mod = Point( vnx, vny ).len();
    double ret = m_par.g - (vn_mod * vn_mod) / m_par.r;

    //cout << "vn_mod = " << vn_mod << endl;
    //cout << "aVert2 = " << ret << ", vn_mod = " << vn_mod << endl;
    return ret;
}
