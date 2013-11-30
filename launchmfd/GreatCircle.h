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

#ifndef GREATCIRCLE_H
#define GREATCIRCLE_H

#include "Visual/Canvas.hpp"
#include "Utils/Pens.h"

namespace EnjoLib
{
struct Point;
}

struct BODYPHYS;
class MFDDataLaunchMFD;

class GreatCircle : public EnjoLib::Canvas
{
    public:
        GreatCircle();
        virtual ~GreatCircle();

        virtual void Update( MyDC hDC, int W, int H, MFDDataLaunchMFD * data );
        void SwitchTrack();
        void SwitchUse();
        virtual void ZoomIn();
        virtual void ZoomOut();
        void SignalPlanetChange( const BODYPHYS & bodyPhys );
        void IncreasePlotPrecision();
        void DecreasePlotPrecision();
        void SetPlotPrecision( double step );
        double GetPlotPrecision() const;

        bool m_continuous;

    protected:
        // Canvas
        void RefreshClient();
        void ScaleData();
        bool IsSymmetricAround00() const;
        EnjoLib::Point GetWindowSize() const;
        EnjoLib::RectangleMy GetDataSize() const;
        EnjoLib::RectangleMy GetDataToDisplaySize() const;
        double GetMinAllowedZoom( double proposedZoom ) const;
        virtual void AutoZoomVirtual();

        const double m_zoomMultipler;
        bool m_track;


    private:
        void DrawCrosses(MyDC hDC, const EnjoLib::Point & pos, Pens::LineStyle lineStyle );
        void DrawCross(MyDC hDC, const EnjoLib::Point & pos );
        void DrawPoint(MyDC hDC, const EnjoLib::Point & pos );
        void DrawLine(MyDC hDC, const EnjoLib::Point & pos1, const EnjoLib::Point & pos2 );
        void DrawVessel( MyDC hDC, const OBJHANDLE hRef, const OBJHANDLE hObj, Pens::LineStyle lineStyle, bool trackSrc );
        void DrawTrajectory(MyDC hDC, const std::vector <EnjoLib::Point> & trajectory, Pens::LineStyle lineStyle );

        Pens m_pens;
        double m_bodyRad;
        static double m_plotStep;
        static double m_plotStepMin, m_plotStepMax;


        bool m_use;
        int m_W, m_H;
};

#endif // GREATCIRCLE_H
