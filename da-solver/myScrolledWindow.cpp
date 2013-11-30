#include "wx_pch.h"

#include "myScrolledWindow.h"
#include "da_solverMain.h"
#include "SourceBody.hpp"
#include "../lib/Math/AzimuthMath.hpp"
#include "../lib/Math/Constants.hpp"
#include <iomanip>

using namespace EnjoLib;

BEGIN_EVENT_TABLE(myScrolledWindow, wxScrolledWindow)

    // EVT_MOTION(myScrolledWindow::OnMouseMove)
    // EVT_LEFT_UP(myScrolledWindow::OnLeftUp)
END_EVENT_TABLE()

// myScrolledWindow dziedziczy z wxScrolledWindow
myScrolledWindow::myScrolledWindow(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style , const wxString& name):
    MyWxScrolledWindow((wxWindow*)parent, id, pos, size, style, name)
{
    m_parent = (da_solverFrame*) parent->GetParent();
    bgCol.Set(230,240,255);
    SetBackgroundColour (bgCol);

    ScrollbarsOn(GetSize());
    Init();
    InitInternalVars();

    Connect(wxID_ANY,wxEVT_SIZE,wxSizeEventHandler(myScrolledWindow::OnSize));

    firstRun = true;
}

myScrolledWindow::~myScrolledWindow()
{
}

void myScrolledWindow::MyDraw (wxDC& dc)
{
    DrawInnerLoop(dc);
}

void myScrolledWindow::GetUserValues()
{
    tgtParam.incl = m_parent->GetInclDeg();
    m_satellite.angEqu = m_parent->GetSatLANOffsetDeg();
    tgtParam.delayRad = m_parent->GetSatDelayDeg();
    latStart = m_parent->GetLatDeg();
    lonStart = m_parent->GetLonDeg();
    timeOffset = m_parent->GetTimeOffset();
    tgtParam.azFlag = m_parent->GetAzimuthSelection() == 0 ? NORTHERN_AZIMUTH : SOUTHERN_AZIMUTH;
    bodyPhys = SourceBody(m_parent->GetBodyType());
    //target_radius = bodyPhys.radius + 350e3;
    target_radius = bodyPhys.radius + 1;
}

void myScrolledWindow::Init()
{
    shipParams.massFull = 24500;
    shipParams.massFlowVert = 7;
    shipParams.massFlowHori = 10;
    shipParams.thrustVert = 280e3;
    shipParams.thrustHori = 400e3;
}

double myScrolledWindow::GetShipsInitialAcceleration()
{
    return 15;
}

bool myScrolledWindow::InitInternalVars()
{
    lonCurrent = lonStart;

    initial_rotation_vel = bodyPhys.radius * (2*PI/bodyPhys.sidDay);
    initial_rotation_vel_wlat = initial_rotation_vel * cos(latStart);
    max_xvel = 0;

    mi = GGRAV_*bodyPhys.mass;  //gravitational parameter
    double azimuth_ref = AzimuthMath().CalcAzimuth( latStart, tgtParam.incl );

    tgt_orbit_v2_ref.y = sqrt(mi/target_radius)*cos(azimuth_ref); // target northern velocity
    tgt_orbit_v2_ref.x = sqrt(mi/target_radius)*sin(azimuth_ref); // target eastern velocity
    tgt_orbit_v2_ref_module = tgt_orbit_v2_ref.len();

    m_satellite.delay = (tgtParam.delayRad + PI/2) * target_radius / tgt_orbit_v2_ref_module;

    // use time offset from slider
    m_satellite.delay -= timeOffset;
    m_satellite.angEqu -= 2*PI / bodyPhys.sidDay * timeOffset;

    CalculateSatellitePosition();

    // Satellite
    UpdateGreatCircleSatellite();
    UpdatePlaneMomentumSatellite();
    // Ship
    UpdateGreatCircleShip();
    UpdatePlaneMomentumShip();
    // For ghosted GC
    m_ship.vGCStart = m_ship.vGC;

    InitInitialShipPosition();

    return true;
}

void myScrolledWindow::AddPathToVectors()
{
    vvs.push_back( m_ship.s );
    vxvel_excessed.push_back( true );
}

void myScrolledWindow::ClearPathVectors()
{
    vvs.clear();
    vxvel_excessed.clear();
}

void myScrolledWindow::Reset()
{
    reset_values();
    GetUserValues();

    InitInternalVars();
    ClearPathVectors();

    ScaleGC();

    if (firstRun)
    {
        CenterView();
        firstRun = false;
    }

}

void myScrolledWindow::DebugCalcAngDist()
{
//	debug = CalcAngDistStartPosToNode() * DEG;
    m_parent->OutputText(OutputValues());
}

void myScrolledWindow::InnerLoop(bool timeEfficient)
{
    static bool bPrepareInnerLoop = false;

    try
    {
        ClearPathVectors();

        if ( bPrepareInnerLoop )
        {
            main_loop_prepare(true);
            UpdateGreatCircleShip();
            ScaleGC( true );
        }

        if ( fabs(Ldistance) > 30 && ! solNotPossible )
        {
            medium_loop();
            bPrepareInnerLoop = false;
        }
        else
        {
            main_loop_finalise();
            bPrepareInnerLoop = true;
        }
        AddPathToVectors();
        ScaleGC( true );
        m_parent->OutputText(OutputValues());
    }
    catch ( exception & e )
    {
        //TODO
        m_parent->OutputText(e.what());
    }
}

void myScrolledWindow::MediumLoop( bool bClean, bool timeEfficient )
{
    ClearPathVectors();
    main_loop_prepare( timeEfficient );

    do
    {
        if (Failed()) break;
        medium_loop();
        if ( ! bClean )
        {
            AddPathToVectors();
        }
    }
    while (fabs(Ldistance) > 50);  // && t1 < 2000);

    if ( bClean )
    {
        AddPathToVectors();
    }
    // Ship
    UpdateGreatCircleShip();

    ScaleGC( true );

    Refresh();
    main_loop_finalise();
}

void myScrolledWindow::CalculateDirectAscent()
{
    Reset();
    do
    {
        ClearPathVectors();
        if (Failed()) break;
        main_loop_prepare( true );
        do
        {
            if (Failed()) break;
            medium_loop();
//            ScalePath();
            AddPathToVectors();
        }
        while (fabs(Ldistance) > 3);  // && t1 < 2000)

        main_loop_finalise();

        // Ship
        UpdateGreatCircleShip();
        ScaleGC( true );

        Refresh();
    }
    while (fabs(distanceShipSatFinal) > 5);
}

bool myScrolledWindow::Failed()
{
    return iter > max_i || solNotPossible;
}

void myScrolledWindow::CalculateDirectAscentFuelEfficient()
{
    Reset();
    m_t.waiting_t = CalcTimeToNodeOnGround();
    do
    {
        if (iter > max_i || fuelEfSolNotPossible) break;
        ClearPathVectors();

        main_loop_prepare( false );
        do
        {
            if (iter > max_i || solNotPossible || fuelEfSolNotPossible) break;
            medium_loop();
//            ScalePath();
            AddPathToVectors();
        }
        while (fabs(Ldistance) > 1);  // && t1 < 2000);

        main_loop_finalise();

        // Ship
        UpdateGreatCircleShip();
        ScaleGC( true );
        Refresh();
    }
    while (fabs(distanceShipSatFinal) > 5);
}

void myScrolledWindow::DrawGC( const std::vector<Point> & gcData, wxDC & dc, const wxPen * pen)
{
    dc.SetPen(*pen);
    const double maxDist = PI/2.0 * target_radius * m_zoom;
    int prevX = 0;
    for ( size_t i = 0; i < gcData.size(); i++ )
    {
        int x = (int)gcData[i].x - m_refSystem.x;
        int y = - (int)(gcData[i].y) - m_refSystem.y;

        if ( prevX != 0 )
        {
            double dist = fabs(gcData[i].x - gcData[i-1].x);
            if ( i == 0 || dist < maxDist)
            {
#if defined(__WXMSW__)
                dc.DrawLine( x, y, x+1, y );
#else
                dc.DrawPoint( x, y );
#endif
            }
        }
        prevX = x;
    }
}

void myScrolledWindow::DrawInnerLoop( wxDC & dc )
{
    dc.SetPen(*wxBLACK_PEN);
    wxSize size = GetClientSize();
    const int w = size.GetWidth();

    dc.SetPen(*wxGREY_PEN);

    // equator
    //int y2 = h - start.y - refSystem.y;
    int y2 = 0 - m_refSystem.y;
    dc.DrawLine( 0, y2, w, y2 );


    // ship's starting GC
    DrawGC(vGCshStartScaled, dc, wxGREY_PEN);
    // ship's GC
    DrawGC(vGCshScaled, dc, wxBLACK_PEN);
    // satellite's GC
    DrawGC(vGCsatScaled, dc, wxBLACK_PEN);


    dc.SetPen(*wxBLACK_PEN);
    // draw satellite's position
    int x = m_satellite.pos2D.x * m_zoom - m_refSystem.x;
    int y = - m_satellite.pos2D.y * m_zoom - m_refSystem.y;
    {
        dc.DrawLine(x - 6 , y, x + 7, y);
        dc.DrawLine(x, y-6,x, y +6);
    }

    // ship's path
    for ( size_t i = 0; i < vvsScaled.size(); i++ )
    {
        DrawGC(vvsScaled[i], dc, wxRED_PEN);
    }

    // Draw asymptote
//    if ( !vvs_x.empty() && !vvs_x[0].empty())
//    {
//        int x1 = vvs_x[0][0] - startX;
//        int y1 = size.GetHeight() - vvs_y[0][0] - startY;
//
//        int x2 = 100 * sin(PI/2.0 - shAsymptoteAzimuth) + x1;
//        int y2 = 100 * cos(PI/2.0 - shAsymptoteAzimuth) + y1;
//        dc.DrawLine( x1, y1, x2, y2);
//	}

}

string myScrolledWindow::CombineStringAndValue(const string & a, double val, const string & unit)
{
    stringstream ss;
    ss << a << ": " << fixed << setprecision(0) << val << " " << unit << "\n";
    return ss.str();
}

wxString myScrolledWindow::OutputValues()
{
    string str;
    stringstream ss;

    if (fuelEfSolNotPossible)
    {
        str = "Fuel efficient solution not possible from this position. \
        Decrease offset to negative vales or change delay or increase inclination";
    }
    else if (solNotPossible)
    {
        string message = "Solution is not possible. We've been waiting for too long for a direct ascent launch, \
        and the satellite didn't manage to a get in a proper position. \
        Try setting a different delay. Station's orbital perios is: ";
        str = CombineStringAndValue(message, m_satellite.orbitPeriod, "s");
    }
    else
    {
        str += CombineStringAndValue("Plane distance", Ldistance, "m");
        str += CombineStringAndValue("Satelite distance", distanceShipSatFinal, "m");
        str += CombineStringAndValue("Max eastern vel.", max_xvel, "m/s");
        str += CombineStringAndValue("Flight time", m_t.total_t - m_t.waiting_t, "s");
        str += CombineStringAndValue("Total time elapsed", m_t.total_t, "s");
        str += CombineStringAndValue("Waiting time", m_t.waiting_t, "s");
        str += CombineStringAndValue("Acc. time", m_t.t1_excession, "s");
        str += CombineStringAndValue("Const vel. flight time", m_t.const_flight_t, "s");
        str += CombineStringAndValue("Tutning time", m_t.turning_t, "s");
        str += CombineStringAndValue("Total DV used", (dvUsed / 1000.0), "km/s");
        str += CombineStringAndValue("Main Iterations", main_iter);
        str += CombineStringAndValue(string("Iterations") + (iter > max_i ? ": gave up after": ""), iter);

        str += CombineStringAndValue("Debug", debug);
    }
    return wxString::From8BitData(str.c_str());
}

void myScrolledWindow::ScrollbarsOn( const wxSize & size )
{
    int unit = 20;
    int pixelsAmount = 6 * size.GetWidth() / unit;
    SetScrollbars(unit, unit, pixelsAmount,  pixelsAmount);
}

// CanvasMovableScallable

void myScrolledWindow::ScaleData()
{
    ScaleGC();
}

void myScrolledWindow::OnSize(wxSizeEvent& event)
{
    CenterView();
}

void myScrolledWindow::ScaleGC( bool bShipOnly )
{
    vvsScaled.clear();
    vvsScaled.assign(vvs.size(), std::vector <Point>() ); // Ensure that sizes of the vector and vectors inside are equal
    for ( size_t i = 0; i < vvs.size() ; ++i )
    {
        // zoom the vectors inside
        ScalePointVector(vvs[i], &vvsScaled[i]);
    }
    ScalePointVector(m_ship.vGCStart, &vGCshStartScaled);
    ScalePointVector(m_ship.vGC, &vGCshScaled);
    if ( ! bShipOnly )
    {
        ScalePointVector(m_satellite.vGC, &vGCsatScaled);
    }
}

void myScrolledWindow::CenterView()
{
    AutoZoom();
}



RectangleMy myScrolledWindow::GetDataToDisplaySize() const
{
    double halfGlobe = PI * bodyPhys.radius;
    return RectangleMy( Point( - halfGlobe, -halfGlobe/2.0 ), Point(  halfGlobe, halfGlobe/2.0 ) );
}

RectangleMy myScrolledWindow::GetDataSize() const
{
    return GetDataToDisplaySize();
}

bool myScrolledWindow::IsSymmetricAround00() const
{
    return true;
}
