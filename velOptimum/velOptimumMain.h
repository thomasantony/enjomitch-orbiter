/***************************************************************
 * Name:      velOptimumMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2010-08-18
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef VELOPTIMUMMAIN_H
#define VELOPTIMUMMAIN_H

#include "../launchmfd/LegacyCode/DirectAscent/OptimalVelSolver.hpp"

#ifndef WX_PRECOMP
//(*Headers(velOptimumFrame)
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)
#endif

class IntegralOnVelWindow;
class VertAccWindow;


class velOptimumFrame: public wxFrame, public OptimalVelSolver
{
    public:

        velOptimumFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~velOptimumFrame();
        virtual VelIntegral FuelOptimIteration( double vel, bool acquireIntegralsFromFirst = false );


    private:
        //double GetIncl();
        double GetDist();
        double GetAsymptote();
        double GetAzimuth();
        double GetStartLatitude();
        void Reset();
        double ReactOnChangedVel( bool acquirePreviousData = false );
        void OutputText( const VelIntegral & solution );

        //(*Handlers(velOptimumFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void Onm_butResetClick(wxCommandEvent& event);
        void Onm_butCalcSolutionClick(wxCommandEvent& event);
        void Onm_sliderVelCmdScroll(wxScrollEvent& event);
        //*)

        //(*Identifiers(velOptimumFrame)
        static const long ID_SCROLLEDWINDOW1;
        static const long ID_SCROLLEDWINDOW2;
        static const long ID_SLIDER1;
        static const long ID_TEXTCTRL1;
        static const long ID_TEXTCTRL5;
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL4;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT1;
        static const long ID_TEXTCTRL3;
        static const long ID_STATICTEXT2;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_BUTTON3;
        static const long ID_PANEL1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(velOptimumFrame)
        wxStaticText* m_statTextAngDist;
        wxButton* m_butCalcSolution;
        wxTextCtrl* m_txtOutput;
        wxTextCtrl* m_txtAzimuth;
        wxStaticText* m_statTextStartLat;
        wxStaticBoxSizer* m_statBoxPermilleOrbVel;
        wxStaticText* m_statTextShipAsymptote;
        wxPanel* Panel1;
        wxSlider* m_sliderVel;
        IntegralOnVelWindow* m_scrWinIntegral;
        wxStatusBar* StatusBar1;
        VertAccWindow* m_scrWinVertAcc;
        wxTextCtrl* m_txtShAsymptote;
        wxStaticBoxSizer* m_statBoxVertAccOnTime;
        wxStaticBoxSizer* m_statBoxIntegralOnVel;
        wxTextCtrl* m_txtStartLat;
        wxButton* m_butReset;
        wxStaticText* m_statTextAzimuth;
        wxButton* m_butCalc;
        wxTextCtrl* m_txtDist;
        //*)


        double m_vel;

        DECLARE_EVENT_TABLE()
};

#endif // VELOPTIMUMMAIN_H
