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
// DirectAscentPage.cpp - data input and output for and from direct ascent algorithm
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

#include <algorithm>
#include "../globals.h"
#include "DirectAscentPage.h"
#include "../../lib/Systems/SystemsConverter.hpp"
#include "../../lib/Math/VectorMath.hpp"
#include "DirectAscentResults.hpp"
#include "../MFDDataLaunchMFD.hpp"

DirectAscentPage::DirectAscentPage (DWORD w, DWORD h, VESSEL *v)
    : MFDPage (w, h, v)
	, m_pidZoom(0.4)
	, m_pidCoordSystem(0.6)
{
	//Center( true );

    InitWords();


}

DirectAscentPage::~DirectAscentPage()
{

}

double DirectAscentPage::GetShipsInitialAcceleration()
{
    return shipParams.thrustHori;
}

void DirectAscentPage::SetMFDData( MFDDataLaunchMFD * data )
{
    this->m_data = data;
	data->Update();
	bodyPhys = data->m_bodyPhys;
	Center( true );
}

void DirectAscentPage::ScheduleForCalculation()
{
    m_bScheduledForCalculation = true;
}

double DirectAscentPage::GetDiffTime()
{
	static double tNow2 = oapiGetSimTime();
	double diffT = oapiGetSimTime() - tNow2;
	tNow2 = oapiGetSimTime();
	return diffT;
}

#ifdef ORB2006
void
#else
bool
#endif
DirectAscentPage::Update ( MyDC hDC )
{
	double diffT = GetDiffTime();
	UpdateZoom(diffT);

	needed_east_v_at_this_alt = sqrt(mi/m_data->m_dRadius) / sqrt(mi/target_radius) * max_xvel ; // proportion ORIGINAL
    //needed_east_v_at_this_alt = max_xvel;
	static bool go = false;
    if ( m_bScheduledForCalculation )
    {
       // oapiSetTimeAcceleration(1); // needed?

        DirectAscentResults results = CalculateDirectAscent();
        m_bScheduledForCalculation = false;
        m_data->SetDAResults(results); // for autopilot
		go = true;
    }

	if ( go )
    {
        static double tNow = oapiGetSimTime();
        if ( oapiGetSimTime() > tNow + 35)
        {
            Point p = SystemsConverter(Geo(m_data->m_dLatitude, m_data->m_dLongitude), m_data->m_dRadius).GetPoint();
			vsRec.push_back(Canvas::m_scalePointFunctor(p));
            tNow = oapiGetSimTime();
        }

        VESSELSTATUS2 vs2;
        vs2.version = 2;
        vs2.flag = 0;
        vessel->GetStatusEx(&vs2);


		diffXFromRotation += diffT * initial_rotation_vel;

        if ( vs2.status == 0 )
        {
			if ( ! alreadyMoved )
            {
				Zoom();
				alreadyMoved = true;
			}

			/*
            if ( ! alreadyMoved )
            {
                double diff = ( (lonCurrent - lonStart) * target_radius ) * m_zoom;
                for ( unsigned i = 0; i < vs.size(); ++i )
                {
                    vs[i].x -= diff;
                }

                for ( unsigned i = 0; i < vGCsat.size(); ++i )
                {
                    vGCsat[i].x -= diff;
                }

                vGCsh.clear();

                alreadyMoved = true;
            }
			*/
        }
        else
        {
            if (alreadyMoved)
			{
				Center(false);
				ScalePath();
				ScaleGC(false);
				alreadyMoved = false;
			}
			UpdateGreatCircleSatellite();
            ScaleGC();
            m_satellite.delay = ( tgtParam.delayRad + PI/2 ) * target_radius / tgt_orbit_v2_ref_module;
            CalculateSatellitePosition();
        }
    }

    char cTgt[BUFLEN];
    strcpy_s(cTgt, sTgt.c_str());
    OBJHANDLE objTarget = oapiGetVesselByName(cTgt);
    VESSEL * target = NULL;
    ELEMENTS elSat;
    ELEMENTS elShip;

    if (objTarget != NULL)
    {
        target = oapiGetVesselInterface(objTarget);
        HANDLE hRef = vessel->GetSurfaceRef();
        target->GetElements (hRef, elSat, NULL, NULL, FRAME_EQU);
        vessel->GetElements (hRef, elShip, NULL, NULL, FRAME_EQU);

        m_satellite.angEqu = CalcAngSatEqu( target );
    }

    DrawInnerLoop(hDC);
    InitOutputVars();
    PrintWords(hDC);

#ifdef ORB2009
    return true;
#endif
}

void DirectAscentPage::PrintWords(MyDC hDC)
{
    int linespacing = W/MFD_LINE_NR;
    int charwidth = H/MFD_CHAR_LEN;
    unsigned int i = 0;
    int line_offset = 0;
    int char_offset = 20;

    if (m_data->GetDAResults().IsValid() )
    {
        DirectAscentPhase currentPhase = m_data->GetDAResults().GetCurrentPhase(oapiGetSimTime());
        for (i=2, line_offset = 1; i < vs_output_da.size() && i < vs_vars_da.size(); i++, line_offset++)
        {
            int xoffset = 20, tab = 0;

            COLORREF nameColor = YELLOW;
            COLORREF varColor = GREEN;
            COLORREF lightBlue = 0x00FFBB22;
            if (i == 2 && currentPhase == DA_PHASE_WAIT) 	nameColor = lightBlue;	// Light blue
            if (i == 3 && currentPhase == DA_PHASE_ACC) 	nameColor = lightBlue;	// Light blue
            if (i == 4 && currentPhase == DA_PHASE_CONST) 	nameColor = lightBlue;	// Light blue
            if (i == 5 && currentPhase == DA_PHASE_ACC_FINAL) 	nameColor = lightBlue;	// Light blue
            // iterations
            if (i == 6 && iter > max_i) varColor = RED;

            MFDTextOut(hDC,charwidth*tab,linespacing*(line_offset), nameColor, vs_output_da[i].c_str());
            MFDTextOut(hDC,charwidth*(xoffset),linespacing*line_offset, varColor, vs_vars_da[i].c_str());

            if (i == 7) line_offset++;	 // one additional space before displaying velocities
        }
    }
}


void DirectAscentPage::SetLineStyle(MyDC hDC, Pens::LineStyle lineStyle )
{
    MFDSetPen(hDC, pens.GetPen(lineStyle) );
}

void DirectAscentPage::DrawTrajectory(MyDC hDC, const std::vector <Point> & trajectory, Pens::LineStyle lineStyle, double diffFromRotation )
{
    SetLineStyle(hDC, lineStyle);
    const double maxDist = PI/2.0 * bodyPhys.radius * m_zoom;
    int prevX = 0;
    int prevY = 0;
    for (unsigned i = 0; i<trajectory.size(); i++)
    {
        int x = (int)(trajectory[i].x - m_refSystem.x - diffFromRotation * m_zoom );
        int y = (int)(- trajectory[i].y - m_refSystem.y);

        if ( prevX != 0)
        {
			const double dist = fabs(trajectory[i].x - trajectory[i-1].x);
            if ( i == 0 || dist < maxDist)
            {
                MFDLine(hDC,x ,y, x + 1 ,y);
            }
        }
        prevX = x;
        prevY = y;
    }
}

void DirectAscentPage::DrawInnerLoop(MyDC hDC)
{
    DrawTrajectory( hDC, vGCsatScaled, Pens::Grey );
    DrawTrajectory( hDC, vGCshScaled, Pens::Grey, diffXFromRotation );
    DrawTrajectory( hDC, vGCshStartScaled, Pens::Grey );
    DrawTrajectory( hDC, vs, Pens::Red, diffXFromRotation );
    DrawTrajectory( hDC, vsRec, Pens::Green );

    SetLineStyle(hDC, Pens::Green);
    //satellite pos
    const int  x = (int)(m_satellite.pos2D.x * m_zoom - m_refSystem.x);
    const int  y = (int)(- m_satellite.pos2D.y * m_zoom - m_refSystem.y) ;
    MFDLine(hDC,x - 6 , y, x + 7, y);
    MFDLine(hDC,x, y-6,x, y +6);
}

void DirectAscentPage::Center( bool reset )
{
	m_zoomTarget = 1/(2*PI * bodyPhys.radius / W);
	m_refSystemTarget = Point(-(int)W/2, -(int)H/2);

	//if (reset)
	{
		m_zoom = m_zoomTarget;
		m_refSystem = m_refSystemTarget;
	}
}
void DirectAscentPage::Zoom()
{
	const Point pStart = SystemsConverter(Geo(latStart, lonStart), bodyPhys.radius).GetPoint();
	const Point pEnd = m_ship.s.back();

	if (pStart.x < pEnd.x)
	{
		double w = pEnd.x - pStart.x;
		m_zoomTarget = W / w * 0.9;
		m_refSystemTarget.x = pStart.x * m_zoomTarget;
		double midPointY = ( MAX(pStart.y, pEnd.y) + MIN(pStart.y, pEnd.y) ) / 2.0;
		m_refSystemTarget.y = - midPointY * m_zoomTarget - H/2;
	}
	else
	{
		// TODO
	}
}

void DirectAscentPage::UpdateZoom(double diffT)
{
	double zoomAdjustment = m_pidZoom.Update(m_zoomTarget - m_zoom, diffT);
	m_zoom += zoomAdjustment;
	ScalePath();
	ScaleGC(false);

	double refSystemAdjustment = m_pidCoordSystem.Update( fabs(m_refSystemTarget.len() - m_refSystem.len()), diffT );
	double alpha = VectorMath().angle( m_refSystemTarget, m_refSystem );
	double refSystemAdjustmentX = (refSystemAdjustment * cos(alpha));
	double refSystemAdjustmentY = (refSystemAdjustment * sin(alpha));

	// Trigonometry handles sign
	m_refSystem.x += refSystemAdjustmentX;
	m_refSystem.y += refSystemAdjustmentY;
}


void DirectAscentPage::ScalePath()
{
	ScalePointVector( m_ship.s, &vs );
}

void DirectAscentPage::ScaleGC( bool bSatOnly )
{
    if ( ! bSatOnly )
    {
		ScalePointVector( m_ship.vGC, &vGCshScaled );
		ScalePointVector( m_ship.vGCStart, &vGCshStartScaled );
    }
	ScalePointVector( m_satellite.vGC, &vGCsatScaled );
}

// Canvas
Point DirectAscentPage::GetWindowSize() const
{
    return Point(W, H);
}

RectangleMy DirectAscentPage::GetDataSize() const
{
    return GetDataToDisplaySize();
}

RectangleMy DirectAscentPage::GetDataToDisplaySize() const
{
   double halfGlobe = PI * bodyPhys.radius;
    return RectangleMy( Point( - halfGlobe, -halfGlobe/2.0 ), Point(  halfGlobe, halfGlobe/2.0 ) );
}
void DirectAscentPage::RefreshClient() {}

void DirectAscentPage::ScaleData()
{
    ScalePath();
    ScaleGC();
}


void DirectAscentPage::SetTargetString(char * cTgt)
{
    sTgt = cTgt;
}


void DirectAscentPage::InnerLoop()
{
    int a = 0;
    do
    {
        if (iter > max_i || solNotPossible)
        {
            a = 1;
            break;
        }
        do
        {
            medium_loop();
            if (iter > max_i || solNotPossible)
            {
                a = 2;
                break;
            }
        }
        while (0);
	}
    while (0);

    if ( a == 1 )
    {
        iter += 10;
    }
    else if ( a == 2 )
    {
        iter += 20;
    }
    else
    {
		//iter = 2999;
    }


    ScalePath();

    UpdateGreatCircleShip();
    UpdateGreatCircleSatellite();

    UpdatePlaneMomentumShip();
    UpdatePlaneMomentumSatellite();

    ScaleGC( false );
    sprintf_s(oapiDebugString(), 255, "Ldistance = %f", Ldistance);
}


DirectAscentResults DirectAscentPage::CalculateDirectAscent()
{
    Reset();
	Center(true);
    UpdateGreatCircleShip();
    UpdateGreatCircleSatellite();

    UpdatePlaneMomentumShip();
    UpdatePlaneMomentumSatellite();

    m_ship.vGCStart = m_ship.vGC;

    int a = 0;
    do
    {
        if (iter > max_i || solNotPossible)
        {
            a = 1;
            break;
        }
        main_loop_prepare(true);
        do
        {
            medium_loop();
            if (iter > max_i || solNotPossible)
            {
                a = 2;
                break;
            }
        }
        while (fabs(Ldistance) > 10);
        main_loop_finalise();
    }
    while (fabs(distanceShipSatFinal) > 40);


    if ( a == 1 )
    {
        iter += 10;
    }
    else if ( a == 2 )
    {
        iter += 20;
    }
    else
    {
    }

    ScalePath();

    UpdateGreatCircleShip();
    UpdateGreatCircleSatellite();

    UpdatePlaneMomentumShip();
    UpdatePlaneMomentumSatellite();

    ScaleGC( false );

    return DirectAscentResults (oapiGetSimTime(), m_t );
}

bool DirectAscentPage::InitInternalVars()
{
    char cTgt[BUFLEN];
    strcpy_s(cTgt, sTgt.c_str());
    OBJHANDLE objTarget = oapiGetVesselByName(cTgt);
    VESSEL * target = NULL;

    if (objTarget == NULL) return false;
    else target = oapiGetVesselInterface(objTarget);
    if (target == NULL || vessel == NULL) return false;
    else
    {
		initial_rotation_vel = bodyPhys.radius * (2*PI/bodyPhys.sidDay);
		initial_rotation_vel_wlat = initial_rotation_vel * cos(m_data->m_dLatitude);
        double tgt_Lon, tgt_Lat;
        target->GetEquPos(tgt_Lon, tgt_Lat, target_radius);

        mi=GGRAV*bodyPhys.mass;  //gravitational parameter
        double azimuth_ref = asin( cos(tgtParam.incl) / cos(m_data->m_dLatitude) );
        tgt_orbit_v2_ref.y = sqrt(mi/target_radius)*cos(azimuth_ref); // target's northern velocity
        tgt_orbit_v2_ref.x = sqrt(mi/target_radius)*sin(azimuth_ref); // target's eastern velocity

        tgt_orbit_v2_ref_module = tgt_orbit_v2_ref.len();
        max_xvel = tgt_orbit_v2_ref_module / 2.0;

        m_satellite.delay = ( tgtParam.delayRad + PI/2 ) * target_radius / tgt_orbit_v2_ref_module;

        // calculating ship's initialAcceleration. Thanks to Agentgonzo for the code
        double Thrust = 0;
        const int numThrusters = vessel->GetGroupThrusterCount(THGROUP_MAIN);
        for(int i = 0; i < numThrusters; i++)
        {
            THRUSTER_HANDLE thruster = vessel->GetGroupThruster(THGROUP_MAIN,i);
            Thrust += vessel->GetThrusterMax0(thruster);
        }
        initialAccel = Thrust / vessel->GetMass();

        lonStart = lonCurrent = m_data->m_dLongitude;
        latStart = m_data->m_dLatitude;

        m_satellite.angEqu = CalcAngSatEqu( target );
		diffXFromRotation = 0;
    }
    return true;
}

bool DirectAscentPage::Reset()
{
    reset_values();
    vsRec.clear();
    alreadyMoved = false;
    return InitInternalVars();
}

double DirectAscentPage::CalcAngSatEqu( VESSEL * target )
{
    ELEMENTS elSat;
    ELEMENTS elShip;

    HANDLE hRef = vessel->GetSurfaceRef();
    target->GetElements (hRef, elSat, NULL, NULL, FRAME_EQU);
    vessel->GetElements (hRef, elShip, NULL, NULL, FRAME_EQU);

    double angSatEqu;
    if ( elShip.theta == 0 )
    {
        angSatEqu = tgtParam.angle;
    }
    else
    {
        angSatEqu = elSat.theta - (elShip.theta + PI/2.0) + lonStart;
    }
    if ( latStart < 0 )
    {
        angSatEqu -= PI;
    }
    return angSatEqu;
}

void DirectAscentPage::InitOutputVars()
{
    std::ostringstream ss_buf;
    vs_vars_da.clear();

    ss_buf.str(std::string());
    ss_buf.precision(1);
    ss_buf.setf(std::ios::fixed);

    double currentTime = oapiGetSimTime();

    ss_buf << m_t.total_t << " s";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());

    ss_buf << m_t.flight_t << " s";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());

    ss_buf << m_data->GetDAResults().GetTimeOfPhase(DA_PHASE_WAIT, currentTime) << " s";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());

    ss_buf << m_data->GetDAResults().GetTimeOfPhase(DA_PHASE_ACC, currentTime) << " s";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());

    ss_buf << m_data->GetDAResults().GetTimeOfPhase(DA_PHASE_CONST, currentTime) << " s";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());

    ss_buf << m_data->GetDAResults().GetTimeOfPhase(DA_PHASE_ACC_FINAL, currentTime) << " s";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());

    ss_buf << iter;
    if (iter > max_i)
        ss_buf << " FAILED";
    else if (solNotPossible)
        ss_buf << " Not possible";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());

    ss_buf << dvUsed / 1000 << " km/s";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());

    ss_buf << daVars.current_vel.len() << " m/s";
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());
    needed_east_v_at_this_alt > 0 ? ss_buf << needed_east_v_at_this_alt << " m/s"
                                           : ss_buf << NA;
    vs_vars_da.push_back(ss_buf.str());
    ss_buf.str(std::string());
}

void DirectAscentPage::InitWords()
{
    if (vs_output_da.empty())
    {
        //vs_output_da.push_back(DIRECT_ASCENT_VIEW);
        vs_output_da.push_back(TOTAL_TIME);
        vs_output_da.push_back(FLIGHT_TIME);
        vs_output_da.push_back(WAITING_TIME);
        vs_output_da.push_back(ACC_EAST_TIME);
        vs_output_da.push_back(CONST_VELOCITY_FLIGHT);
        vs_output_da.push_back(TURNING_TIME);
        vs_output_da.push_back(ITERATIONS);
        vs_output_da.push_back(DV_USED);
        vs_output_da.push_back(CURRENT_E_VEL);
        vs_output_da.push_back(ACCELERATE_TO);
    }
}


