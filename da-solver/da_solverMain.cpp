/***************************************************************
 * Name:      da_solverMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Szymon Ender ()
 * Created:   2009-12-20
 * Copyright: Szymon Ender ()
 * License:  GPLv3
 **************************************************************/

#include "wx_pch.h"

#include "da_solverMain.h"

#ifndef WX_PRECOMP
#include <wx/msgdlg.h>

//(*InternalHeaders(da_solverFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#endif

#include "myScrolledWindow.h"
#include "SourceBody.hpp"
#include "../lib/Math/Constants.hpp"


//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

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

bool timeEfficient = true;

//(*IdInit(da_solverFrame)
const long da_solverFrame::ID_SCROLLEDWINDOW1 = wxNewId();
const long da_solverFrame::ID_SLIDER1 = wxNewId();
const long da_solverFrame::ID_BUTTON8 = wxNewId();
const long da_solverFrame::ID_TEXTCTRL5 = wxNewId();
const long da_solverFrame::ID_TEXTCTRL2 = wxNewId();
const long da_solverFrame::ID_STATICTEXT1 = wxNewId();
const long da_solverFrame::ID_TEXTCTRL3 = wxNewId();
const long da_solverFrame::ID_STATICTEXT2 = wxNewId();
const long da_solverFrame::ID_TEXTCTRL4 = wxNewId();
const long da_solverFrame::ID_STATICTEXT3 = wxNewId();
const long da_solverFrame::ID_TEXTCTRL6 = wxNewId();
const long da_solverFrame::ID_STATICTEXT5 = wxNewId();
const long da_solverFrame::ID_TEXTCTRL7 = wxNewId();
const long da_solverFrame::ID_STATICTEXT6 = wxNewId();
const long da_solverFrame::ID_RADIOBOX2 = wxNewId();
const long da_solverFrame::ID_CHOICE1 = wxNewId();
const long da_solverFrame::ID_BUTTON2 = wxNewId();
const long da_solverFrame::ID_BUTTON3 = wxNewId();
const long da_solverFrame::ID_BUTTON4 = wxNewId();
const long da_solverFrame::ID_BUTTON5 = wxNewId();
const long da_solverFrame::ID_BUTTON6 = wxNewId();
const long da_solverFrame::ID_BUTTON7 = wxNewId();
const long da_solverFrame::ID_PANEL1 = wxNewId();
const long da_solverFrame::idMenuQuit = wxNewId();
const long da_solverFrame::ID_MENU_INNER_LOOP = wxNewId();
const long da_solverFrame::ID_MENU_MEDIUM_LOOP = wxNewId();
const long da_solverFrame::ID_MENU_MEDIUM_LOOP_CLEAN = wxNewId();
const long da_solverFrame::idMenuAbout = wxNewId();
const long da_solverFrame::ID_MENU_INSTRUCTIONS = wxNewId();
const long da_solverFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(da_solverFrame,wxFrame)
    //(*EventTable(da_solverFrame)
    //*)
END_EVENT_TABLE()

da_solverFrame::da_solverFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(da_solverFrame)
    wxBoxSizer* BoxSizer4;
    wxStaticBoxSizer* StaticBoxSizer2;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;
    wxStaticText* StaticText2;
    wxMenuItem* MenuItem2;
    wxStaticText* StaticText6;
    wxMenuItem* MenuItem1;
    wxStaticText* StaticText1;
    wxBoxSizer* BoxSizer2;
    wxStaticText* StaticText3;
    wxMenu* Menu1;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxGridSizer* GridSizer1;
    wxStaticText* StaticText5;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* MenuBar1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;

    Create(parent, wxID_ANY, _("Direct ascent algorithm visualisation for Launch MFD"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(923,734));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(168,216), wxSize(720,450), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("Visualisation"));
    m_scrolledGraph = new myScrolledWindow(Panel1, ID_SCROLLEDWINDOW1, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL, _T("ID_SCROLLEDWINDOW1"));
    StaticBoxSizer1->Add(m_scrolledGraph, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("Time offset"));
    m_sliderTimeOffset = new wxSlider(Panel1, ID_SLIDER1, 0, -20000, 20000, wxDefaultPosition, wxDefaultSize, wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER1"));
    StaticBoxSizer3->Add(m_sliderTimeOffset, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 5);
    BoxSizer2->Add(StaticBoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("View"));
    m_butCenter = new wxButton(Panel1, ID_BUTTON8, _("Center"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    StaticBoxSizer2->Add(m_butCenter, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(StaticBoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer5, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 5, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    m_txtOutput = new wxTextCtrl(Panel1, ID_TEXTCTRL5, _("Output"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    BoxSizer4->Add(m_txtOutput, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(BoxSizer4, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
    m_txtIncl = new wxTextCtrl(Panel1, ID_TEXTCTRL2, _("52"), wxDefaultPosition, wxSize(60,20), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    FlexGridSizer1->Add(m_txtIncl, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Inclination °"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_txtSatLANOffset = new wxTextCtrl(Panel1, ID_TEXTCTRL3, _("0"), wxDefaultPosition, wxSize(60,20), 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    FlexGridSizer1->Add(m_txtSatLANOffset, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Orbit offset +/-°"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_txtSatDelay = new wxTextCtrl(Panel1, ID_TEXTCTRL4, _("0"), wxDefaultPosition, wxSize(60,20), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    FlexGridSizer1->Add(m_txtSatDelay, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Pos in orbit +/-°"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer1->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_txtLat = new wxTextCtrl(Panel1, ID_TEXTCTRL6, _("28"), wxDefaultPosition, wxSize(60,20), 0, wxDefaultValidator, _T("ID_TEXTCTRL6"));
    FlexGridSizer1->Add(m_txtLat, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("Latitude +/-°"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer1->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_txtLon = new wxTextCtrl(Panel1, ID_TEXTCTRL7, _("-90"), wxDefaultPosition, wxSize(60,20), 0, wxDefaultValidator, _T("ID_TEXTCTRL7"));
    FlexGridSizer1->Add(m_txtLon, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(Panel1, ID_STATICTEXT6, _("Longitude +/-°"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer1->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(FlexGridSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    wxString __wxRadioBoxChoices_1[2] =
    {
    	_("Ascending"),
    	_("Descending")
    };
    m_radioBoxAzimuth = new wxRadioBox(Panel1, ID_RADIOBOX2, _("Azimuth"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 1, 0, wxDefaultValidator, _T("ID_RADIOBOX2"));
    BoxSizer3->Add(m_radioBoxAzimuth, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    m_choiceBody = new wxChoice(Panel1, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
    BoxSizer6->Add(m_choiceBody, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(BoxSizer6, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridSizer1 = new wxGridSizer(3, 2, 0, 0);
    m_butMedLoop = new wxButton(Panel1, ID_BUTTON2, _("Main loop"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    GridSizer1->Add(m_butMedLoop, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_butSolveTimeEf = new wxButton(Panel1, ID_BUTTON3, _("Solution"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    GridSizer1->Add(m_butSolveTimeEf, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_butMedClean = new wxButton(Panel1, ID_BUTTON4, _("Main clean"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    GridSizer1->Add(m_butMedClean, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_butSolveFuelEf = new wxButton(Panel1, ID_BUTTON5, _("Sol Fuel Ef"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    GridSizer1->Add(m_butSolveFuelEf, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_butInnerLoop = new wxButton(Panel1, ID_BUTTON6, _("Medium loop"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    GridSizer1->Add(m_butInnerLoop, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_butRestart = new wxButton(Panel1, ID_BUTTON7, _("Restart"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
    GridSizer1->Add(m_butRestart, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(GridSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(BoxSizer1);
    BoxSizer1->SetSizeHints(Panel1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Exit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu3, ID_MENU_INNER_LOOP, _("Medium\tF2"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem3);
    MenuItem5 = new wxMenuItem(Menu3, ID_MENU_MEDIUM_LOOP, _("Main\tF3"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem5);
    MenuItem4 = new wxMenuItem(Menu3, ID_MENU_MEDIUM_LOOP_CLEAN, _("Main clean\tF4"), wxEmptyString, wxITEM_NORMAL);
    Menu3->Append(MenuItem4);
    MenuBar1->Append(Menu3, _("Loop"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuItem6 = new wxMenuItem(Menu2, ID_MENU_INSTRUCTIONS, _("Instructions"), _("F1"), wxITEM_NORMAL);
    Menu2->Append(MenuItem6);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    m_statusBar = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    m_statusBar->SetFieldsCount(1,__wxStatusBarWidths_1);
    m_statusBar->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(m_statusBar);

    Connect(ID_SLIDER1,wxEVT_SCROLL_TOP|wxEVT_SCROLL_BOTTOM|wxEVT_SCROLL_LINEUP|wxEVT_SCROLL_LINEDOWN|wxEVT_SCROLL_PAGEUP|wxEVT_SCROLL_PAGEDOWN|wxEVT_SCROLL_THUMBTRACK|wxEVT_SCROLL_THUMBRELEASE|wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&da_solverFrame::Onm_sliderTimeOffsetCmdScroll);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&da_solverFrame::Onm_butCenterClick);
    Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&da_solverFrame::Onm_txtInclText);
    Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&da_solverFrame::Onm_txtSatLANOffsetText);
    Connect(ID_TEXTCTRL4,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&da_solverFrame::Onm_txtSatDelayText);
    Connect(ID_TEXTCTRL6,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&da_solverFrame::Onm_txtLatText);
    Connect(ID_TEXTCTRL7,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&da_solverFrame::Onm_txtLonText);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&da_solverFrame::Onm_butMedLoopClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&da_solverFrame::Onm_butSolveTimeEfClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&da_solverFrame::Onm_butMedCleanClick);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&da_solverFrame::Onm_butSolveFuelEfClick);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&da_solverFrame::Onm_butInnerLoopClick);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&da_solverFrame::Onm_butRestartClick1);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&da_solverFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&da_solverFrame::OnAbout);
    //*)

    Connect(ID_SLIDER1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&da_solverFrame::Onm_sliderTimeOffsetCmdScroll);
    Connect(ID_SLIDER1,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&da_solverFrame::Onm_sliderTimeOffsetCmdScroll);


    Connect(ID_MENU_INNER_LOOP,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&da_solverFrame::Onm_butInnerLoopClick);
    Connect(ID_MENU_MEDIUM_LOOP,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&da_solverFrame::Onm_butMedLoopClick);
    Connect(ID_MENU_MEDIUM_LOOP_CLEAN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&da_solverFrame::Onm_butMedCleanClick);
    Connect(ID_MENU_INSTRUCTIONS,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&da_solverFrame::OnInstructions);


    //wxSize s = m_scrolledGraph->GetSize();

    m_bAlreadyReset = false;
    MapBody mapBody;
    for (unsigned i = 0; i < mapBody.size(); ++i)
       m_choiceBody->Append("");
    for ( MapBody::iterator it = mapBody.begin(); it != mapBody.end(); ++it) {
        m_choiceBody->SetString(it->first, it->second);
    }
    m_choiceBody->SetSelection(0);

}

da_solverFrame::~da_solverFrame()
{
    //(*Destroy(da_solverFrame)
    //*)
}

void da_solverFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void da_solverFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void da_solverFrame::OnInstructions(wxCommandEvent& event)
{
  wxMessageBox(_("Remember to press Restart upon changing any of parameters\n"
                 "and upon program start.\n\n- moving the mouse with right click held moves viewpoint"
                 "\n\n- mouse wheel zooms viewpoint to the place where the mouse cursor is"),
               _("Instructions") );
}


void da_solverFrame::Onm_butMedLoopClick(wxCommandEvent& event)
{
  ResetIfNeeded();
  m_scrolledGraph->MediumLoop( false, timeEfficient );

  wxString str = m_scrolledGraph->OutputValues();
  m_txtOutput->SetValue(str);
  //m_scrolledGraph->Refresh();
}

void da_solverFrame::Onm_butMedCleanClick(wxCommandEvent& event)
{
  ResetIfNeeded();
  m_scrolledGraph->MediumLoop( true, timeEfficient );

  wxString str = m_scrolledGraph->OutputValues();
  m_txtOutput->SetValue(str);
  //m_scrolledGraph->Refresh();
}


void da_solverFrame::Onm_butSolveTimeEfClick(wxCommandEvent& event)
{
  m_scrolledGraph->CalculateDirectAscent();
//  m_scrolledGraph->DrawInnerLoop();
  wxString str = m_scrolledGraph->OutputValues();
  m_txtOutput->SetValue(str);
  //m_scrolledGraph->Refresh();
}

void da_solverFrame::Onm_butSolveFuelEfClick(wxCommandEvent& event)
{
  m_scrolledGraph->CalculateDirectAscentFuelEfficient();
  wxString str = m_scrolledGraph->OutputValues();
  m_txtOutput->SetValue(str);
}

void da_solverFrame::Onm_butInnerLoopClick(wxCommandEvent& event)
{
  ResetIfNeeded();
  m_scrolledGraph->InnerLoop(true);
  wxString str = m_scrolledGraph->OutputValues();
  //m_txtOutput->SetValue(str);
  m_scrolledGraph->Refresh();

}

void da_solverFrame::Onm_butRestartClick1(wxCommandEvent& event)
{
  m_scrolledGraph->Reset();
  m_scrolledGraph->Refresh();
}

void da_solverFrame::OutputText( const wxString & str )
{
  m_txtOutput->SetValue(str);
}

void da_solverFrame::Onm_txtSatLANOffsetText(wxCommandEvent& event)
{
  //MakeEventStringNumber( event );
}

void da_solverFrame::Onm_txtSatDelayText(wxCommandEvent& event)
{
  //MakeEventStringNumber( event );
}

void da_solverFrame::Onm_txtInclText(wxCommandEvent& event)
{
  //MakeEventStringNumber( event );
//  int maxIncl = 150;
//  long int incl = maxIncl + 1;
//  if ( event.GetString().ToLong( &incl ) == true )
//  {
//    // may fail
//  }
//  if ( incl > maxIncl )
//  {
//    m_txtIncl->SetValue( wxString::Format(_T("%d"),maxIncl) );
//  }
}

int da_solverFrame::GetAzimuthSelection()
{
  return m_radioBoxAzimuth->GetSelection();
}


void da_solverFrame::MakeEventStringNumber(wxCommandEvent& event, bool bPositive )
{

  bool found = false;
  bool dotFound = false;
  long pos = -1;  // where to set insertion point of txt ctrl?
  wxString str = event.GetString();

  for (size_t i = 0; i < str.Len(); i ++ )
  {
    wxChar c = str.GetChar(i);
    if ( (c < '0' || c > '9') )
    {
      if ( c == '.' )
      {
        if ( dotFound ) // if found a second dot
        {
          found = true;
        }
        else
        {
          dotFound = true;
          continue;
        }

      }
      if ( (!bPositive && i == 0 && c == '-') )
      {
        continue; // allow - at 0th position
      }
       pos = i;
      // dispose the offending char
      str = str.Mid(0, i) + str.Mid(i + 1); // str is gluing str from begining to i and from i+1 to the end
      found = true;
      break;
    }
  }

  if ( found )
  {
    if ( event.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED )
    {
      wxTextCtrl * ptxt = (wxTextCtrl *)event.GetEventObject();
      ptxt->SetValue(str);
      if ( pos >= 0 )
        ptxt->SetInsertionPoint(pos);
      else
        ptxt->SetInsertionPointEnd();
    }

  }
}



double da_solverFrame::GetInclDeg()
{
  double ret = 75;
  m_txtIncl->GetValue().ToDouble( &ret );
  return ret * RAD;
}
double da_solverFrame::GetSatLANOffsetDeg()
{
  double ret = 0;
  m_txtSatLANOffset->GetValue().ToDouble( &ret );
  return ret * RAD;
}
double da_solverFrame::GetSatDelayDeg()
{
  double ret = 0;
  m_txtSatDelay->GetValue().ToDouble( &ret );
  return ret * RAD;
}

double da_solverFrame::GetLatDeg()
{
  double ret = 0;
  m_txtLat->GetValue().ToDouble( &ret );
  return ret * RAD;
}

double da_solverFrame::GetLonDeg()
{
  double ret = 0;
  m_txtLon->GetValue().ToDouble( &ret );
  return ret * RAD;
}

SourceBody::BODY_TYPE da_solverFrame::GetBodyType()
{
    return (SourceBody::BODY_TYPE)m_choiceBody->GetSelection();
}

double da_solverFrame::GetTimeOffset()
{
  return m_sliderTimeOffset->GetValue();
}


void da_solverFrame::Onm_txtLatText(wxCommandEvent& event)
{
  MakeEventStringNumber( event );
}

void da_solverFrame::Onm_txtLonText(wxCommandEvent& event)
{
  MakeEventStringNumber( event );
}

void da_solverFrame::ResetIfNeeded()
{
  if ( ! m_bAlreadyReset )
  {
    m_scrolledGraph->Reset();
    m_scrolledGraph->Refresh();
    m_bAlreadyReset = true;
  }
}

void da_solverFrame::Onm_sliderTimeOffsetCmdScroll(wxScrollEvent& event)
{
  m_scrolledGraph->Reset();
  m_scrolledGraph->Refresh();

  //DEBUG
  m_scrolledGraph->DebugCalcAngDist();

}

void da_solverFrame::Onm_butCenterClick(wxCommandEvent& event)
{
    m_scrolledGraph->CenterView();
}
