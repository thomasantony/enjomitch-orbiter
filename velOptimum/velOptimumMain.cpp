/***************************************************************
 * Name:      velOptimumMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2010-08-18
 * Copyright:  ()
 * License:
 **************************************************************/
#include "wx_pch.h"

#include "velOptimumMain.h"

#ifndef WX_PRECOMP
#include <wx/msgdlg.h>
//(*InternalHeaders(velOptimumFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#endif
#include "../lib/Math/Constants.hpp"

#include "IntegralOnVelWindow.h"
#include "VertAccWindow.h"
#include <iostream>
#include <typeinfo>
#include <cmath>

using namespace std;

//double simpson_int (double a, double b, const void * data, double (*function)(double x, const void * data), double epsilon = 0.001);
//class CaVertParams;


//helper functions
enum wxbuildinfoformat
{
    short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(velOptimumFrame)
const long velOptimumFrame::ID_SCROLLEDWINDOW1 = wxNewId();
const long velOptimumFrame::ID_SCROLLEDWINDOW2 = wxNewId();
const long velOptimumFrame::ID_SLIDER1 = wxNewId();
const long velOptimumFrame::ID_TEXTCTRL1 = wxNewId();
const long velOptimumFrame::ID_TEXTCTRL5 = wxNewId();
const long velOptimumFrame::ID_STATICTEXT4 = wxNewId();
const long velOptimumFrame::ID_TEXTCTRL4 = wxNewId();
const long velOptimumFrame::ID_STATICTEXT3 = wxNewId();
const long velOptimumFrame::ID_TEXTCTRL2 = wxNewId();
const long velOptimumFrame::ID_STATICTEXT1 = wxNewId();
const long velOptimumFrame::ID_TEXTCTRL3 = wxNewId();
const long velOptimumFrame::ID_STATICTEXT2 = wxNewId();
const long velOptimumFrame::ID_BUTTON1 = wxNewId();
const long velOptimumFrame::ID_BUTTON2 = wxNewId();
const long velOptimumFrame::ID_BUTTON3 = wxNewId();
const long velOptimumFrame::ID_PANEL1 = wxNewId();
const long velOptimumFrame::idMenuQuit = wxNewId();
const long velOptimumFrame::idMenuAbout = wxNewId();
const long velOptimumFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(velOptimumFrame,wxFrame)
    //(*EventTable(velOptimumFrame)
    //*)
END_EVENT_TABLE()

velOptimumFrame::velOptimumFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(velOptimumFrame)
    wxBoxSizer* BoxSizer4;
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxBoxSizer* BoxSizer2;
    wxMenu* Menu1;
    wxGridSizer* GridSizer1;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;
    wxGridSizer* GridSizer2;

    Create(parent, wxID_ANY, _("Calculating optimal flight velocity for direct ascent program"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(743,506));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(152,200), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    m_statBoxIntegralOnVel = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("Total dv on maximal velocity reached"));
    m_scrWinIntegral = new IntegralOnVelWindow(Panel1, ID_SCROLLEDWINDOW1, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW1"));
    m_statBoxIntegralOnVel->Add(m_scrWinIntegral, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(m_statBoxIntegralOnVel, 3, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_statBoxVertAccOnTime = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("Vertical acceleration on time of transfer"));
    m_scrWinVertAcc = new VertAccWindow(Panel1, ID_SCROLLEDWINDOW2, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW2"));
    m_statBoxVertAccOnTime->Add(m_scrWinVertAcc, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(m_statBoxVertAccOnTime, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_statBoxPermilleOrbVel = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("‰ of orbital velocity"));
    m_sliderVel = new wxSlider(Panel1, ID_SLIDER1, 0, 0, 1000, wxDefaultPosition, wxDefaultSize, wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER1"));
    m_statBoxPermilleOrbVel->Add(m_sliderVel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(m_statBoxPermilleOrbVel, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 5, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    m_txtOutput = new wxTextCtrl(Panel1, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(200,-1), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer4->Add(m_txtOutput, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(BoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1 = new wxGridSizer(0, 2, 0, 0);
    m_txtStartLat = new wxTextCtrl(Panel1, ID_TEXTCTRL5, _("28"), wxDefaultPosition, wxSize(40,-1), wxTE_CENTRE, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    m_txtStartLat->SetToolTip(_("Starting latitude\nInfluences only the body\'s rotation, \nthus the initial velocity."));
    GridSizer1->Add(m_txtStartLat, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_statTextStartLat = new wxStaticText(Panel1, ID_STATICTEXT4, _("Start latitude"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    GridSizer1->Add(m_statTextStartLat, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_txtShAsymptote = new wxTextCtrl(Panel1, ID_TEXTCTRL4, _("120"), wxDefaultPosition, wxSize(40,-1), wxTE_CENTRE, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    m_txtShAsymptote->SetToolTip(_("Ship\'s asymptote\nDefines the asymptote of ship\'s velocity in the position\nof interception. If it is equal to azimuth, then you won\'t need any plane change. Setting it to 0 or below has no\nsense here, because it would mean that you are on \na polar, or a retrograde orbit.\n"));
    GridSizer1->Add(m_txtShAsymptote, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_statTextShipAsymptote = new wxStaticText(Panel1, ID_STATICTEXT3, _("shAsymptote"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    GridSizer1->Add(m_statTextShipAsymptote, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_txtAzimuth = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("70"), wxDefaultPosition, wxSize(40,-1), wxTE_CENTRE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    m_txtAzimuth->SetToolTip(_("Required azimuth\nAzimuth of acceleration required to equalise orbits."));
    GridSizer1->Add(m_txtAzimuth, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_statTextAzimuth = new wxStaticText(Panel1, ID_STATICTEXT1, _("Azimuth"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    GridSizer1->Add(m_statTextAzimuth, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_txtDist = new wxTextCtrl(Panel1, ID_TEXTCTRL3, _("15"), wxDefaultPosition, wxSize(40,-1), wxTE_CENTRE, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    m_txtDist->SetToolTip(_("Angular distance\nDefines the distance to the satellite\'s plane, in degrees.\nReal distance is (Radius * angle * PI / 180)\nNote that for bigger distances, it\'s optimal to reach the \norbital velocity after all."));
    GridSizer1->Add(m_txtDist, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_statTextAngDist = new wxStaticText(Panel1, ID_STATICTEXT2, _("Ang distance"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    GridSizer1->Add(m_statTextAngDist, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(GridSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer2 = new wxGridSizer(2, 2, 0, 0);
    m_butCalc = new wxButton(Panel1, ID_BUTTON1, _("Iteration"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    m_butCalc->SetToolTip(_("One iteration"));
    GridSizer2->Add(m_butCalc, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_butReset = new wxButton(Panel1, ID_BUTTON2, _("Reset"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    m_butReset->SetToolTip(_("Gets user values"));
    GridSizer2->Add(m_butReset, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_butCalcSolution = new wxButton(Panel1, ID_BUTTON3, _("Solution"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    m_butCalcSolution->SetToolTip(_("Final solution"));
    GridSizer2->Add(m_butCalcSolution, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(GridSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(BoxSizer1);
    BoxSizer1->Fit(Panel1);
    BoxSizer1->SetSizeHints(Panel1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Connect(ID_SLIDER1,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&velOptimumFrame::Onm_sliderVelCmdScroll);
    Connect(ID_SLIDER1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&velOptimumFrame::Onm_sliderVelCmdScroll);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&velOptimumFrame::OnButton1Click);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&velOptimumFrame::Onm_butResetClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&velOptimumFrame::Onm_butCalcSolutionClick);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&velOptimumFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&velOptimumFrame::OnAbout);
    //*)

    // Earth
    m_bodyPhys.mass = 5.973698968e24;
    m_bodyPhys.radius = 6.37101e6;
    m_bodyPhys.sidDay = 86164.09;
    //m_pars.target_radius = m_bodyPhys.radius + 300e3;
    m_pars.target_radius = m_bodyPhys.radius + 30e3;

    m_shipPars.massFull = 24500;
    m_shipPars.massFlowVert = 7;
    m_shipPars.massFlowHori = 10;
    m_shipPars.thrustVert = 280e3;
    m_shipPars.thrustHori = 400e3;

    SetClientSize(wxSize(750,600));

}

velOptimumFrame::~velOptimumFrame()
{
    //(*Destroy(velOptimumFrame)
    //*)
}

void velOptimumFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void velOptimumFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg =
        _T("This program is a visualisation of the velocity solver for my Direct ascent program "\
           "for Orbiter Spaceflight Simulator's Launch MFD module\n\n"\
           "The transfer consists of (currently) 3 phases:\n"\
           "1) Accelerating east to reach a given velocity < orbital velocity\n"\
           "2) Constant velocity flight\n"\
           "3) Bootleg manouevre to equalise velocity vector with the target\n\n"\
           "Ad. 1) During the 1st phase, the ship accelerates with the main engines, while using the hovers "\
           "to counter the gravity. The more velocity you build up, the greater the centrifugal force, "\
           "so the less you have to be accelerating with the hovers.\n\n"\
           "Ad. 2) In the 2nd phase you also need to be countering the resulting gravity, based on the "\
           "velocity that you have built up, while covering the required distance.\n\n"\
           "Ad. 3) In the last phase, the greater your velocity was the use more fuel you will use "\
           "to equalise the velocity vector. Note, that if there's a big angular difference in orbital "\
           "planes and your velocity was big, then you will have to lessen the velocity vector initially, "\
           "which will result in lessening your centrifugal force, finally requiring that you use the "\
           "hovers more.\n\n"
           "The lower window displays vertical acceleration of all three phases as a function of time. "\
           "The upper window displays delta v's as a function of the velocity reached in the 1st phase.\n"\
           "I strongly recommend using the velocity slider, which will help you visualise the problem. "\
           "The windows are somewhat scalable and the input text edit boxes have important tooltips.\n"
           "Before doing any calculations, press the Reset button, to get the user values and iterate "\
           "the velocity for plotting the dv.\n\n"\
           "Copyright © 2010 Szymon Ender\nLicensed under GNU GPL v3");
    wxMessageBox(msg, _("Direct ascent optimal velocity solver help."));
}

void velOptimumFrame::OutputText( const VelIntegral & solution )
{
    m_txtOutput->Clear();
    m_txtOutput->AppendText(_T("Final velocity:"));
    m_txtOutput->AppendText( wxString::Format(_T("\n%.2f%\n%.2f m/s\n\n"), solution.vel / GetTrueVelOrb(), solution.vel ) );
    m_txtOutput->AppendText( wxString::Format(_T("dv used:\nvertical = %.2f m/s\nhorizontal = %.2f m/s\n"), solution.dvVert, solution.dvHori  ));
    m_txtOutput->AppendText(_T("total = "));
    m_txtOutput->AppendText( wxString::Format(_T("%.2f m/s\n\n"), solution.dv() ));
    m_txtOutput->AppendText( wxString::Format(_T("Times of manouevres:\nInitial acc = %.2f s\nConst vel flight = %.2f s\nAcc to equalise = %.2f s\n"), t_acc, t_const, t_final ));
    m_txtOutput->AppendText(_T("Yotal = "));
    m_txtOutput->AppendText( wxString::Format(_T("%.2f s\n\n"), t_acc + t_const + t_final ));
    m_txtOutput->AppendText(_T("Const vel flight acting gravity:"));
    m_txtOutput->AppendText( wxString::Format(_T("\n%.2f m/s^2\n\n"), av_const ));
    //m_txtOutput->AppendText( wxString::Format(_T("Debug :\nIterations = %d\nAngle diff = %.2f\n\n"), m_iter, m_angDiff * DEG));

    //m_txtOutput->SetValue( wxString::Format(_T("av_const  = %.2f, vGraphAccValues = %.2f"), av_const , vGraphAccValues.front().y ));
//
//    m_txtOutput->AppendText( wxString::Format(_T("\n%.2f m/s^2"), test1 ));
//    m_txtOutput->AppendText( wxString::Format(_T("\n%.2f m/s^2"), test2 ));
//    m_txtOutput->AppendText( wxString::Format(_T("\n%.2f m/s^2"), test1 + test2 ));
}

void velOptimumFrame::Onm_butCalcSolutionClick(wxCommandEvent& event)
{
//  Reset();
    VelIntegral result = CalcFuelOptim( m_vel, 200 );

    // for plotting
    m_vel = result.vel / GetTrueVelOrb();
    ReactOnChangedVel();
}

void velOptimumFrame::Onm_sliderVelCmdScroll(wxScrollEvent& event)
{
    m_iter = 0;
    m_vel = event.GetInt() / 1000.0; // per mille
    ReactOnChangedVel( true );
    //m_txtOutput->SetValue( wxString::Format(_T("Vel = %.2f"), event.GetInt() / 100.0 ) );
}


double velOptimumFrame::ReactOnChangedVel( bool acquirePreviousData )
{
    VelIntegral res = FuelOptimIteration( m_vel, acquirePreviousData );
    double vel = res.vel;
    if ( ! acquirePreviousData )
        res.vel *= GetTrueVelOrb();
    else
        res.vel = m_vel * GetTrueVelOrb();

    OutputText(res);
    m_scrWinVertAcc->SetCurrentSolutionsFunctionValues( vGraphAccValues, vGraphEqualizingValues );
    m_scrWinVertAcc->SetConstantAcceleration( av_const );
// wxMessageBox(wxString::Format(_T("av_const  = %.2f, vGraphAccValues = %.2f"), av_const , vGraphAccValues.front().y ));
    m_scrWinVertAcc->Refresh();
    m_scrWinIntegral->SetCurrentSolution( res );

    m_scrWinIntegral->Refresh();

    return vel;
}

VelIntegral velOptimumFrame::FuelOptimIteration( double vel, bool acquireIntegralsFromFirst )
{
    VelIntegral ret = OptimalVelSolver::FuelOptimIteration( vel, acquireIntegralsFromFirst );

    if ( ! acquireIntegralsFromFirst )
    {
        VelIntegral retUpd = CalcIntegralFromVel( ret.vel, true );
        ret.dvVert = retUpd.dvVert;
        ret.dvHori = retUpd.dvHori;
    }
    return ret;
}

void velOptimumFrame::OnButton1Click(wxCommandEvent& event)
{
    m_vel = ReactOnChangedVel(); // feedback
}

double velOptimumFrame::GetAsymptote()
{
    double ret = 0;
    m_txtShAsymptote->GetValue().ToDouble( &ret );
    return ret * RAD;
}

double velOptimumFrame::GetStartLatitude()
{
    double ret = 0;
    m_txtStartLat->GetValue().ToDouble( &ret );
    return ret * RAD;
}

double velOptimumFrame::GetAzimuth()
{
    double ret = 0;
    m_txtAzimuth->GetValue().ToDouble( &ret );
    return ret * RAD;
}

double velOptimumFrame::GetDist()
{
    double ret = 0;
    m_txtDist->GetValue().ToDouble( &ret );
    return ret * RAD;
}

void velOptimumFrame::Onm_butResetClick(wxCommandEvent& event)
{
    Reset();
}

void velOptimumFrame::Reset()
{

    m_scrWinIntegral->Reset();
    m_scrWinVertAcc->Reset();

    m_iter = 0;

    //incl = GetIncl();
    m_pars.shAsymptoteAzimuth = GetAsymptote();
    m_pars.azimuth = GetAzimuth();
    m_pars.latStart = GetStartLatitude();
    m_pars.angDist = GetDist();// * 1000 / m_bodyPhys.radius; // convert to meters

    m_vel = 0;

    do
    {
        m_vel += 0.025; // iterating
        VelIntegral base = CalcIntegralFromVel( m_vel );
        m_scrWinIntegral->Add( base );
    }
    while ( m_vel < 2 );

    m_scrWinIntegral->SetOrbitalVelocity( GetTrueVelOrb() );

    m_scrWinIntegral->Refresh();

    m_vel = 0.5;
}


