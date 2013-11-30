/***************************************************************
 * Name:      proj_testMain.h
 * Purpose:   Defines Application Frame
 * Author:    Simon Ender ()
 * Created:   2008-11-06
 * Copyright: Simon Ender ()
 * License:
 **************************************************************/

#ifndef PROJ_TESTMAIN_H
#define PROJ_TESTMAIN_H

//(*Headers(proj_testFrame)
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/calctrl.h>
#include <wx/panel.h>
#include <wx/filedlg.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)
#include <wx/databaselayer/MysqlDatabaseLayer.h>
#include "WorkListCtrl.h"
#include "ProjListCtrl.h"

class proj_testFrame: public wxFrame
{
    public:

        proj_testFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~proj_testFrame();
        void SelectAllEverywhere();

        MysqlDatabaseLayer db;

    private:

        //(*Handlers(proj_testFrame)
        void OnOpen(wxCommandEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnNotebook1PageChanged(wxNotebookEvent& event);
        void OnbProjNewClick(wxCommandEvent& event);
        void OnbProjModifyClick(wxCommandEvent& event);
        void OnpProjDeleteClick(wxCommandEvent& event);
        void OnbWorkNewClick(wxCommandEvent& event);
        void OnbWorkDeleteClick(wxCommandEvent& event);
        void OnbWorkModifyClick(wxCommandEvent& event);
        //*)
        void OnSelectedWorkers(wxListEvent & event);
        void OnActivatedWorkers(wxListEvent & event);
        void OnSelectedProjects(wxListEvent & event);
        void OnActivatedProjects(wxListEvent & event);
        void OnConnect(wxCommandEvent & event);
        //(*Identifiers(proj_testFrame)
        static const long ID_LISTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_BUTTON3;
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT2;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL1;
        static const long ID_TEXTCTRL2;
        static const long ID_TEXTCTRL3;
        static const long ID_PANEL1;
        static const long ID_LISTCTRL2;
        static const long ID_BUTTON4;
        static const long ID_BUTTON5;
        static const long ID_BUTTON6;
        static const long ID_STATICTEXT5;
        static const long ID_STATICTEXT6;
        static const long ID_TEXTCTRL4;
        static const long ID_TEXTCTRL5;
        static const long ID_CALENDARCTRL1;
        static const long ID_PANEL2;
        static const long ID_NOTEBOOK1;
        static const long idMenuConnect;
        static const long idMenuDefCol;
        static const long idMenuAutoCol;
        static const long idMenuEnterWidth;
        static const long idMenuGrid;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(proj_testFrame)
        wxCalendarCtrl* Calendar;
        wxMenuItem* MenuItem8;
        wxTextCtrl* txtPRevenue;
        wxMenuItem* MenuItem7;
        wxNotebook* Notebook1;
        wxMenuItem* MenuItem5;
        wxMenu* Menu3;
        wxMenuItem* MenuItem4;
        wxPanel* Panel1;
        wxFileDialog* FileDialog1;
        wxMenuItem* MenuItem10;
        wxTextCtrl* txtSurname;
        WorkListCtrl* lWorkers;
        wxMenuItem* MenuItem3;
        wxTextCtrl* txtCity;
        wxStatusBar* StatusBar1;
        wxMenuItem* MenuItem6;
        ProjListCtrl* lProjects;
        wxTextCtrl* txtPName;
        wxTextCtrl* txtName;
        wxPanel* Panel2;
        wxMenuItem* MenuItem9;
        //*)
        long wid, pid; // worker id, project id

        DECLARE_EVENT_TABLE()


};

#endif // PROJ_TESTMAIN_H
