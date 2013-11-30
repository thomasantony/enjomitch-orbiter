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
// GreatCircle - Calculates and draws great circles, Map MFD-style
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

#include "GreatCircle.h"
#include "MFDDataLaunchMFD.hpp"

#include <algorithm> //for std::swap
#include <Math/GreatCircleAdvMath.hpp>
#include <Math/GreatCircleMath.hpp>
#include <Math/GeneralMath.hpp>
#include <Math/SpaceMathBody.hpp>
#include <Orbiter/SystemsConverterOrbiter.hpp>
#include <Orbiter/SpaceMathOrbiter.hpp>
#include <Systems/Geo.hpp>
#include <Systems/Point.hpp>

using namespace EnjoLib;

double GreatCircle::m_plotStep = 0.01;
double GreatCircle::m_plotStepMin = 0.0025;
double GreatCircle::m_plotStepMax = 0.125;

GreatCircle::GreatCircle()
//    : m_zoomMultipler( 1.75 )
    : m_zoomMultipler( 1.012 )
{
    m_track = true;
    m_use = true;
    m_continuous = false;
}

GreatCircle::~GreatCircle()
{
}

void GreatCircle::SwitchUse()
{
    m_use = ! m_use;
}

void GreatCircle::SwitchTrack()
{
    m_track = ! m_track;
    AutoZoomVirtual();
}

void GreatCircle::ZoomIn()
{
    if ( m_track )
        m_zoom *= m_zoomMultipler;
}

void GreatCircle::ZoomOut()
{
    if ( m_track )
    {
        double newValue = m_zoom / m_zoomMultipler;
        m_zoom = GetMinAllowedZoom( newValue );
    }
}

double GreatCircle::GetMinAllowedZoom( double proposedZoom ) const
{
    double minAllowedZoom = CalculateAutoZoomValue();
    if ( proposedZoom < minAllowedZoom )
        return minAllowedZoom; // don't exceed the minimal allowed zoom
    else
        return proposedZoom;
}

void GreatCircle::SignalPlanetChange( const BODYPHYS & bodyPhys )
{
    m_bodyRad = bodyPhys.radius;
    AutoZoomVirtual();
}

void GreatCircle::AutoZoomVirtual()
{
    Canvas::AutoZoom();
}

void GreatCircle::Update( MyDC hDC, int W, int H, MFDDataLaunchMFD * data )
{
    if ( ! m_use )
        return;

    if ( m_W != W || m_H != H )
    {
        m_W = W;
        m_H = H;
        AutoZoomVirtual();
    }

    m_bodyRad = data->m_bodyPhys.radius;

    DrawVessel( hDC, data->hRef, data->GetVessel()->GetHandle(),    Pens::Green, true );
    DrawVessel( hDC, data->hRef, data->GetTargetHandle(),           Pens::Yellow, false );
}

void GreatCircle::DrawVessel( MyDC hDC, const OBJHANDLE hRef, const OBJHANDLE hObj, Pens::LineStyle lineStyle, bool trackSrc )
{
    if ( ! hObj )
        return;

    const SpaceMathOrbiter::ElementsOrbitParam & elop = SpaceMathOrbiter().GetElements( hObj, hRef, FRAME_EQU );

    const double radius = oapiGetSize(hRef);
    const double planetAngle = oapiGetPlanetCurrentRotation(hRef);
    const double highestPoint = elop.el.theta + PI/2.0; // of the great circle - 90* forward from LAN
    const double highestPointRotating = highestPoint - planetAngle;

    // Find position in orbit
    double positionInOrbit = elop.op.TrL - highestPoint; // with relation to the highest point
    while (positionInOrbit < 0)
        positionInOrbit += 2*PI;

    const Geo & geoPoint = GreatCircleMath().CalcPointOnWholeGC(positionInOrbit/(2*PI), Geo(elop.el.i, highestPointRotating) );
    Point pos2D = SystemsConverterOrbiter( geoPoint, radius ).GetPoint();

    if ( m_track && trackSrc )
    {
        m_refSystem.x = pos2D.x * m_zoom - GetWindowSize().x/2;
        m_refSystem.y = -pos2D.y * m_zoom - GetWindowSize().y/2;
    }

    // Draw position in orbit
    DrawCrosses(hDC, pos2D, lineStyle);
    // Draw Great Circle
    const std::vector<Point> & gc = GreatCircleAdvMath().CalculateGreatCircle(radius, elop.el.i, highestPointRotating, false, m_plotStep);
    DrawTrajectory(hDC, gc, lineStyle);
}

void GreatCircle::DrawCrosses(MyDC hDC, const Point & pos, Pens::LineStyle lineStyle )
{
    Point t = Point( pos.x, -pos.y );
    Point offset( 2 * PI * m_bodyRad, 0 );
    MFDSetPen( hDC, m_pens.GetPen(lineStyle) );
    DrawCross(hDC, t);
    DrawCross(hDC, t - offset);
    DrawCross(hDC, t + offset);
}

void GreatCircle::DrawCross(MyDC hDC, const Point & pos )
{
    Point p = Canvas::GetPointProjected(pos);
    int x = (int)p.x;
    int y = (int)p.y;
    MFDLine(hDC,x - 6 ,y, x + 7, y);
    MFDLine(hDC,x, y - 6, x, y + 6);
}

void GreatCircle::DrawPoint(MyDC hDC, const Point & pos )
{
    const Point & p = Canvas::GetPointProjected(pos);
    MFDPoint(hDC, (int)p.x , (int)p.y);
}

void GreatCircle::DrawLine(MyDC hDC, const Point & pos1, const Point & pos2 )
{
    const Point & pdiff = pos1 - pos2;
    if ( fabs(pdiff.x) >= PI * m_bodyRad)
        return;
    const Point & p1 = Canvas::GetPointProjected(pos1);
    const Point & p2 = Canvas::GetPointProjected(pos2);
    MFDLine(hDC, (int)p1.x , (int)p1.y, (int)p2.x , (int)p2.y);
}
void GreatCircle::DrawTrajectory(MyDC hDC, const std::vector <Point> & trajectory, Pens::LineStyle lineStyle )
{
    MFDSetPen( hDC, m_pens.GetPen(lineStyle) );
    const Point offset( 2 * PI * m_bodyRad, 0 );
    for (unsigned i = 1; i < trajectory.size(); i++)
    {
        Point p1 = Point( trajectory[i].x, -trajectory[i].y );
        Point p2 = Point( trajectory[i-1].x, -trajectory[i-1].y );
        Point offset( 2 * PI * m_bodyRad, 0 );
        if (m_continuous)
        {
            DrawLine(hDC, p1, p2);
            DrawLine(hDC, p1 - offset, p2 - offset);
            DrawLine(hDC, p1 + offset, p2 + offset);
        }
        else
        {
            DrawPoint(hDC, p1);
            DrawPoint(hDC, p1 - offset);
            DrawPoint(hDC, p1 + offset);
        }
    }
}

void GreatCircle::IncreasePlotPrecision()
{
    m_plotStep /= 2;
    if ( m_plotStep < m_plotStepMin )
        m_plotStep = m_plotStepMin;
}
void GreatCircle::DecreasePlotPrecision()
{
    m_plotStep *= 2;
    if ( m_plotStep > m_plotStepMax )
        m_plotStep = m_plotStepMax;
}
void GreatCircle::SetPlotPrecision( double step )
{
    m_plotStep = step;
}
double GreatCircle::GetPlotPrecision() const
{
    return m_plotStep;
}

// Canvas functions
Point GreatCircle::GetWindowSize() const
{
    return Point(m_W, m_H);
}

RectangleMy GreatCircle::GetDataSize() const
{
    return GetDataToDisplaySize();
}

RectangleMy GreatCircle::GetDataToDisplaySize() const
{
    double halfGlobeX = PI * m_bodyRad;
    double halfGlobeY = PI * m_bodyRad / 2.0;
    Point halfGlobe(halfGlobeX, halfGlobeY);
    return RectangleMy( -halfGlobe, halfGlobe );
}

bool GreatCircle::IsSymmetricAround00() const
{
    return true;
}

// refreshed externally
void GreatCircle::RefreshClient()
{
}

void GreatCircle::ScaleData( )
{
}
