/***************************************************************
 * Name:      da_solverMain.h
 * Purpose:   Defines Application Frame
 * Author:    Szymon Ender ()
 * Created:   2009-12-20
 * Copyright: Szymon Ender ()
 * License:
 **************************************************************/

#ifndef DA_SOLVERMAIN_H
/***************************************************************
 * Name:      da_solverApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Szymon Ender ()
 * Created:   2009-12-20
 * Copyright: Szymon Ender ()
 * License: GPLv3
 **************************************************************/

#define DA_SOLVERMAIN_H

#ifndef WX_PRECOMP

//(*Headers(da_solverFrame)
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)
#endif

#include "SourceBody.hpp"

class myScrolledWindow;

class da_solverFrame: public wxFrame
{
    public:

        da_solverFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~da_solverFrame();

        double GetInclDeg();
        double GetSatLANOffsetDeg();
        double GetSatDelayDeg();
        double GetLatDeg();
        double GetLonDeg();
        double GetTimeOffset();
        int GetAzimuthSelection();
        SourceBody::BODY_TYPE GetBodyType();

        void OutputText( const wxString & str );


    private:

        //(*Handlers(da_solverFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void Onm_butMedLoopClick(wxCommandEvent& event);
        void Onm_butSolveTimeEfClick(wxCommandEvent& event);
        void Onm_butRestartClick(wxCommandEvent& event);
        void Onm_butInnerLoopClick(wxCommandEvent& event);
        void Onm_butRestartClick1(wxCommandEvent& event);
        void Onm_txtSatLANOffsetText(wxCommandEvent& event);
        void Onm_txtInclText(wxCommandEvent& event);
        void Onm_txtSatDelayText(wxCommandEvent& event);
        void Onm_butSolveFuelEfClick(wxCommandEvent& event);
        void Onm_butMedCleanClick(wxCommandEvent& event);
        void Onm_txtLatText(wxCommandEvent& event);
        void Onm_txtLonText(wxCommandEvent& event);
        void Onm_sliderTimeOffsetCmdScroll(wxScrollEvent& event);
        void Onm_butCenterClick(wxCommandEvent& event);
        //*)

        void OnInstructions(wxCommandEvent& event);
        void ResetIfNeeded();


        void MakeEventStringNumber(wxCommandEvent& event, bool bPositive = false);

        //(*Identifiers(da_solverFrame)
        static const long ID_SCROLLEDWINDOW1;
        static const long ID_SLIDER1;
        static const long ID_BUTTON8;
        static const long ID_TEXTCTRL5;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT1;
        static const long ID_TEXTCTRL3;
        static const long ID_STATICTEXT2;
        static const long ID_TEXTCTRL4;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL6;
        static const long ID_STATICTEXT5;
        static const long ID_TEXTCTRL7;
        static const long ID_STATICTEXT6;
        static const long ID_RADIOBOX2;
        static const long ID_CHOICE1;
        static const long ID_BUTTON2;
        static const long ID_BUTTON3;
        static const long ID_BUTTON4;
        static const long ID_BUTTON5;
        static const long ID_BUTTON6;
        static const long ID_BUTTON7;
        static const long ID_PANEL1;
        static const long idMenuQuit;
        static const long ID_MENU_INNER_LOOP;
        static const long ID_MENU_MEDIUM_LOOP;
        static const long ID_MENU_MEDIUM_LOOP_CLEAN;
        static const long idMenuAbout;
        static const long ID_MENU_INSTRUCTIONS;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(da_solverFrame)
        wxButton* m_butMedClean;
        wxButton* m_butSolveFuelEf;
        wxChoice* m_choiceBody;
        wxTextCtrl* m_txtOutput;
        wxMenuItem* MenuItem5;
        wxMenu* Menu3;
        wxMenuItem* MenuItem4;
        wxStatusBar* m_statusBar;
        wxTextCtrl* m_txtSatLANOffset;
        wxSlider* m_sliderTimeOffset;
        wxRadioBox* m_radioBoxAzimuth;
        wxButton* m_butCenter;
        wxPanel* Panel1;
        wxTextCtrl* m_txtIncl;
        wxTextCtrl* m_txtLon;
        wxMenuItem* MenuItem3;
        wxMenuItem* MenuItem6;
        myScrolledWindow* m_scrolledGraph;
        wxButton* m_butRestart;
        wxTextCtrl* m_txtLat;
        wxButton* m_butMedLoop;
        wxButton* m_butSolveTimeEf;
        wxTextCtrl* m_txtSatDelay;
        wxButton* m_butInnerLoop;
        //*)

        bool m_bAlreadyReset;

        DECLARE_EVENT_TABLE()
};

#endif // DA_SOLVERMAIN_H
