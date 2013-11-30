/***************************************************************
 * Name:      proj_testMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Simon Ender ()
 * Created:   2008-11-06
 * Copyright: Simon Ender ()
 * License:
 **************************************************************/

#include "proj_testMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(proj_testFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/databaselayer/DatabaseLayerException.h>

#include "WorkAssProjDialog.h"
#include "ProjAssWorkDialog.h"
#include "ConnectDialog.h"
#include "Globals.h"

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

//(*IdInit(proj_testFrame)
const long proj_testFrame::ID_LISTCTRL1 = wxNewId();
const long proj_testFrame::ID_BUTTON1 = wxNewId();
const long proj_testFrame::ID_BUTTON2 = wxNewId();
const long proj_testFrame::ID_BUTTON3 = wxNewId();
const long proj_testFrame::ID_STATICTEXT1 = wxNewId();
const long proj_testFrame::ID_STATICTEXT2 = wxNewId();
const long proj_testFrame::ID_STATICTEXT3 = wxNewId();
const long proj_testFrame::ID_TEXTCTRL1 = wxNewId();
const long proj_testFrame::ID_TEXTCTRL2 = wxNewId();
const long proj_testFrame::ID_TEXTCTRL3 = wxNewId();
const long proj_testFrame::ID_PANEL1 = wxNewId();
const long proj_testFrame::ID_LISTCTRL2 = wxNewId();
const long proj_testFrame::ID_BUTTON4 = wxNewId();
const long proj_testFrame::ID_BUTTON5 = wxNewId();
const long proj_testFrame::ID_BUTTON6 = wxNewId();
const long proj_testFrame::ID_STATICTEXT5 = wxNewId();
const long proj_testFrame::ID_STATICTEXT6 = wxNewId();
const long proj_testFrame::ID_TEXTCTRL4 = wxNewId();
const long proj_testFrame::ID_TEXTCTRL5 = wxNewId();
const long proj_testFrame::ID_CALENDARCTRL1 = wxNewId();
const long proj_testFrame::ID_PANEL2 = wxNewId();
const long proj_testFrame::ID_NOTEBOOK1 = wxNewId();
const long proj_testFrame::idMenuConnect = wxNewId();
const long proj_testFrame::idMenuDefCol = wxNewId();
const long proj_testFrame::idMenuAutoCol = wxNewId();
const long proj_testFrame::idMenuEnterWidth = wxNewId();
const long proj_testFrame::idMenuGrid = wxNewId();
const long proj_testFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(proj_testFrame,wxFrame)
    //(*EventTable(proj_testFrame)
    //*)
END_EVENT_TABLE()

proj_testFrame::proj_testFrame(wxWindow* parent,wxWindowID id)
{
    try
    {
        db.Open(_("localhost"), _("test"));
        db.RunQuery(CREATE_WORKERS, true);
        db.RunQuery(CREATE_PROJECTS, true);
        db.RunQuery(CREATE_ASSIGNMENTS, true);
    }
    catch (DatabaseLayerException & e)
    {
        wxMessageBox(e.GetErrorMessage(), _("Exc proj_testFrame::proj_testFrame"));
#ifdef __WXMSW__
  // windows makes segfaults otherwise
        Close();
        
#endif 
    }

    //(*Initialize(proj_testFrame)
    wxBoxSizer* BoxSizer4;
    wxStaticBoxSizer* StaticBoxSizer2;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer10;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* BoxSizer8;
    wxButton* bProjModify;
    wxStaticText* StaticText2;
    wxMenuItem* MenuItem2;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxStaticText* StaticText6;
    wxMenuItem* MenuItem1;
    wxButton* bWorkNew;
    wxStaticText* StaticText1;
    wxBoxSizer* BoxSizer2;
    wxStaticText* StaticText3;
    wxBoxSizer* BoxSizer11;
    wxMenu* Menu1;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxButton* bProjNew;
    wxStaticText* StaticText5;
    wxButton* pProjDelete;
    wxButton* bWorkDelete;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer9;
    wxMenuBar* MenuBar1;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxBoxSizer* BoxSizer3;
    wxMenu* Menu2;
    wxStaticBoxSizer* StaticBoxSizer5;
    wxButton* bWorkModify;

    Create(parent, wxID_ANY, _("MySQL database"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(647,567));
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxPoint(0,104), wxSize(647,356), 0, _T("ID_NOTEBOOK1"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    lWorkers = new WorkListCtrl(Panel1, ID_LISTCTRL1, wxDefaultPosition, wxSize(633,211), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES|wxSUNKEN_BORDER, wxDefaultValidator, _T("ID_LISTCTRL1"));
    lWorkers->SetHelpText(_("Workers list"));
    BoxSizer1->Add(lWorkers, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("Data manipulation"));
    bWorkNew = new wxButton(Panel1, ID_BUTTON1, _("New"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    StaticBoxSizer1->Add(bWorkNew, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bWorkModify = new wxButton(Panel1, ID_BUTTON2, _("Modify"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    StaticBoxSizer1->Add(bWorkModify, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bWorkDelete = new wxButton(Panel1, ID_BUTTON3, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    StaticBoxSizer1->Add(bWorkDelete, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer3, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, Panel1, _("Fields"));
    BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Surname"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer10->Add(StaticText1, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    BoxSizer10->Add(StaticText2, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("City"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    BoxSizer10->Add(StaticText3, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer10, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    txtSurname = new wxTextCtrl(Panel1, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer11->Add(txtSurname, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtName = new wxTextCtrl(Panel1, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    BoxSizer11->Add(txtName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtCity = new wxTextCtrl(Panel1, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    BoxSizer11->Add(txtCity, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer11, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer4, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer2, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(BoxSizer1);
    BoxSizer1->Fit(Panel1);
    BoxSizer1->SetSizeHints(Panel1);
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer5 = new wxBoxSizer(wxVERTICAL);
    lProjects = new ProjListCtrl(Panel2, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_HRULES|wxLC_VRULES|wxSUNKEN_BORDER, wxDefaultValidator, _T("ID_LISTCTRL2"));
    BoxSizer5->Add(lProjects, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("Data manipulation"));
    bProjNew = new wxButton(Panel2, ID_BUTTON4, _("New"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    StaticBoxSizer3->Add(bProjNew, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    bProjModify = new wxButton(Panel2, ID_BUTTON5, _("Modify"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON5"));
    StaticBoxSizer3->Add(bProjModify, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    pProjDelete = new wxButton(Panel2, ID_BUTTON6, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    StaticBoxSizer3->Add(pProjDelete, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer8->Add(StaticBoxSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxVERTICAL, Panel2, _("Fields"));
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    StaticText5 = new wxStaticText(Panel2, ID_STATICTEXT5, _("Name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    BoxSizer7->Add(StaticText5, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(Panel2, ID_STATICTEXT6, _("Revenue"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    BoxSizer7->Add(StaticText6, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(BoxSizer7, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    txtPName = new wxTextCtrl(Panel2, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    BoxSizer9->Add(txtPName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    txtPRevenue = new wxTextCtrl(Panel2, ID_TEXTCTRL5, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    BoxSizer9->Add(txtPRevenue, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4->Add(BoxSizer9, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer8->Add(StaticBoxSizer4, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer6->Add(BoxSizer8, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(BoxSizer6, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, Panel2, _("Deadline"));
    Calendar = new wxCalendarCtrl(Panel2, ID_CALENDARCTRL1, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxCAL_MONDAY_FIRST|wxCAL_SHOW_HOLIDAYS|wxSUNKEN_BORDER, _T("ID_CALENDARCTRL1"));
    Calendar->SetHelpText(_("Specify deadline"));
    StaticBoxSizer5->Add(Calendar, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer5->Add(StaticBoxSizer5, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    Panel2->SetSizer(BoxSizer5);
    BoxSizer5->Fit(Panel2);
    BoxSizer5->SetSizeHints(Panel2);
    Notebook1->AddPage(Panel1, _("Workers"), false);
    Notebook1->AddPage(Panel2, _("Projects"), false);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, wxID_OPEN, _("&Open\tCtrl+o"), _("Open database"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem4 = new wxMenuItem(Menu1, idMenuConnect, _("&Connect\tCtrl+c"), _("Connect to database"), wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    Menu1->AppendSeparator();
    MenuItem5 = new wxMenuItem(Menu1, wxID_PRINT, _("&Print\tCtrl+p"), _("Print selection"), wxITEM_NORMAL);
    Menu1->Append(MenuItem5);
    Menu1->AppendSeparator();
    MenuItem1 = new wxMenuItem(Menu1, wxID_EXIT, _("&Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem6 = new wxMenuItem(Menu3, idMenuDefCol, _("&Default width"), _("Default columns\' width"), wxITEM_NORMAL);
    Menu3->Append(MenuItem6);
    MenuItem7 = new wxMenuItem(Menu3, idMenuAutoCol, _("&Automatic width"), _("Automatic column width"), wxITEM_NORMAL);
    Menu3->Append(MenuItem7);
    MenuItem8 = new wxMenuItem(Menu3, idMenuEnterWidth, _("&Enter width"), _("Enter columns\' width"), wxITEM_NORMAL);
    Menu3->Append(MenuItem8);
    Menu3->AppendSeparator();
    MenuItem9 = new wxMenuItem(Menu3, idMenuGrid, _("Disable &grid\tCtrl+g"), _("Switch grid in lists"), wxITEM_CHECK);
    Menu3->Append(MenuItem9);
    MenuBar1->Append(Menu3, _("&View"));
    Menu2 = new wxMenu();
    MenuItem10 = new wxMenuItem(Menu2, wxID_HELP, _("&Usage\tF1"), _("How to use the program"), wxITEM_NORMAL);
    Menu2->Append(MenuItem10);
    MenuItem2 = new wxMenuItem(Menu2, wxID_ABOUT, _("&About\tCtrl+a"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    FileDialog1 = new wxFileDialog(this, _("Select file"), wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxFileDialog"));

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&proj_testFrame::OnbWorkNewClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&proj_testFrame::OnbWorkModifyClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&proj_testFrame::OnbWorkDeleteClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&proj_testFrame::OnbProjNewClick);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&proj_testFrame::OnbProjModifyClick);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&proj_testFrame::OnpProjDeleteClick);
    Connect(ID_NOTEBOOK1,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&proj_testFrame::OnNotebook1PageChanged);
    Connect(wxID_EXIT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&proj_testFrame::OnQuit);
    Connect(wxID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&proj_testFrame::OnAbout);
    //*)

    wid = pid = 0;
    Connect(wxID_OPEN,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&proj_testFrame::OnOpen);

    Connect(ID_LISTCTRL1, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(proj_testFrame::OnSelectedWorkers));
    Connect(ID_LISTCTRL1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(proj_testFrame::OnActivatedWorkers));
    Connect(ID_LISTCTRL2, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(proj_testFrame::OnSelectedProjects));
    Connect(ID_LISTCTRL2, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(proj_testFrame::OnActivatedProjects));

    Connect(idMenuConnect, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(proj_testFrame::OnConnect));

}



proj_testFrame::~proj_testFrame()
{
    //(*Destroy(proj_testFrame)
    //*)
    db.Close();
}

void proj_testFrame::OnConnect(wxCommandEvent & event)
{
    new ConnectDialog(this, _T("DB connection info"), &db);
    SelectAllEverywhere();
}

void proj_testFrame::SelectAllEverywhere()
{
  lProjects->SelectAll();
  lWorkers->SelectAll();
}

void proj_testFrame::OnSelectedWorkers(wxListEvent & event)
{
    wid = lWorkers->GetItemData(event.m_itemIndex);
    wxListItem info;
    info.m_itemId = event.m_itemIndex;
    info.m_col = 0;
    info.m_mask = wxLIST_MASK_TEXT;

    //ident->m_text->SetLabel(wxString::Format(wxT("%d"), count));

    if ( lWorkers->GetItem(info) )
    {
      txtSurname->Clear();
      txtSurname->WriteText(info.m_text);
    }
    info.m_col ++;
    if ( lWorkers->GetItem(info) )
    {
      txtName->Clear();
      txtName->WriteText(info.m_text);
    }
    info.m_col ++;
    if ( lWorkers->GetItem(info) )
    {
      txtCity->Clear();
      txtCity->WriteText(info.m_text);
    }
}

void proj_testFrame::OnActivatedWorkers(wxListEvent & event)
{
  new WorkAssProjDialog(this, wxT("Assignments"), &db, wid);
 // dialog->Show(true);
}

void proj_testFrame::OnSelectedProjects(wxListEvent & event)
{
    pid = lProjects->GetItemData(event.m_itemIndex);
    wxListItem info;
    info.m_itemId = event.m_itemIndex;
    info.m_col = 0;
    info.m_mask = wxLIST_MASK_TEXT;

    //ident->m_text->SetLabel(wxString::Format(wxT("%d"), count));

    if ( lProjects->GetItem(info) )
    {
      txtPName->Clear();
      txtPName->WriteText(info.m_text);
    }
    info.m_col = 2;
    if ( lProjects->GetItem(info) )
    {
      txtPRevenue->Clear();
      txtPRevenue->WriteText(info.m_text);
    }

    wxString query = wxString::Format(_("SELECT deadline FROM projects WHERE pid = %d;"), pid);

   try {
      DatabaseResultSet * q = db.RunQueryWithResults(query);
      if ( q->Next() )
      {
          wxDateTime date = q->GetResultDate( _("deadline") );
          if ( date.IsValid() )   Calendar->SetDate( date );
          else                    wxMessageBox(_("Invalid date"));
          db.CloseResultSet( q );
      }
    }
    catch (DatabaseLayerException & e)
    {
        wxMessageBox(e.GetErrorMessage(), _("Exc MyListCtrl::Select"));
    }
    Calendar->GetDate();

}
void proj_testFrame::OnActivatedProjects(wxListEvent & event)
{
  new ProjAssWorkDialog(this, wxT("Assignments"), &db, pid);
}

void proj_testFrame::OnOpen(wxCommandEvent& event)
{
    FileDialog1->Show();
}

void proj_testFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void proj_testFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void proj_testFrame::OnNotebook1PageChanged(wxNotebookEvent& event)
{

}

void proj_testFrame::OnbProjNewClick(wxCommandEvent& event)
{
  wxString buf, query = _T("INSERT INTO projects (name, deadline, revenue) VALUES ('");
  if ( txtPName->IsEmpty() )
  {
    wxMessageBox(_("Name field empty"), _("Error") , wxOK | wxICON_ERROR );
    return;
  }
  query += txtPName->GetLineText(0);
  query += _T("', '");
  wxDateTime date = Calendar->GetDate();
  buf.Printf( _("%d-%d-%d"), date.GetYear(), date.GetMonth() + 1, date.GetDay() );
  query += buf;
  //query += date.FormatDate();
  query += _T("', ");
  query += txtPRevenue->GetLineText(0);
  query += _T("); ");

  txtPName->Clear();
  txtPRevenue->Clear();

  lProjects->Query(query);
  lProjects->SelectAll();
}

void proj_testFrame::OnbProjModifyClick(wxCommandEvent& event)
{
  wxString buf;
  wxString query = _T("UPDATE projects SET name='");
  if ( txtPName->IsEmpty() )
  {
    wxMessageBox(_("Name field empty"), _("Error") , wxOK | wxICON_ERROR );
    return;
  }

  query += txtPName->GetLineText(0);
  query += _T("', revenue=");
  query += txtPRevenue->GetLineText(0);
  query += _(", deadline = '");
  wxDateTime date = Calendar->GetDate();
  buf.Printf( _("%d-%d-%d"), date.GetYear(), date.GetMonth() + 1, date.GetDay() );
  query += buf;
  //query += date.FormatDate();
  query += _T("' WHERE pid=");
  buf.Printf(_T("%d;"), pid);
  query += buf;

  lProjects->Query(query);
  lProjects->SelectAll();

}

void proj_testFrame::OnpProjDeleteClick(wxCommandEvent& event)
{
  wxString buf;
  wxString query = _T("SELECT name FROM projects WHERE pid=");
  buf.Printf(_T("%d;"), pid);
  query += buf;
  try {
    wxString question = _("Are you sure you want to delete project:\n");
    DatabaseResultSet * q = db.RunQueryWithResults(query);
    if (q->Next())
    {
      question += q->GetResultString(_("name")) + _(" ?");
      db.CloseResultSet(q);
    } else return;

    int result = wxMessageBox(question, _T("question"), wxYES_NO);
    if (result == wxNO) return;

    query = _T("DELETE FROM projects WHERE pid=");
    buf.Printf(_T("%d;"), pid);
    query += buf;

    lProjects->Query(query);
    lProjects->SelectAll();

    // Not needed, since we CASCADE foreign keys
    // query = _T("DELETE FROM assignments WHERE userid=");
    // buf.Printf(_T("%d"), m_userid);
    // query += buf + _T(";");
    // UpdateQuery(query);
  }
  catch (DatabaseLayerException & e)
  {
      wxMessageBox(e.GetErrorMessage(), _("Exc OnbProjDeleteClick"));
  }
}

void proj_testFrame::OnbWorkNewClick(wxCommandEvent& event)
{
  wxString query = _T("INSERT INTO workers (surname, name, city) VALUES ('");
  if ( txtSurname->IsEmpty() )
  {
    wxMessageBox(_("Surname field empty"), _("Error") , wxOK | wxICON_ERROR );
    return;
  }
  query += txtSurname->GetLineText(0);
  query += _T("', '");
  query += txtName->GetLineText(0);
  query += _T("', '");
  query += txtCity->GetLineText(0);
  query += _T("'); ");

  txtSurname->Clear();
  txtName->Clear();
  txtCity->Clear();

  lWorkers->Query(query);
  lWorkers->SelectAll();
}

void proj_testFrame::OnbWorkModifyClick(wxCommandEvent& event)
{
  wxString buf;
  wxString query = _T("UPDATE workers SET surname='");
  if ( txtSurname->IsEmpty() )
  {
    wxMessageBox(_("Surname field empty"), _("Error") , wxOK | wxICON_ERROR );
    return;
  }
  query += txtSurname->GetLineText(0);
  query += _T("', name='");
  query += txtName->GetLineText(0);
  query += _T("', city='");
  query += txtCity->GetLineText(0);
  query += _T("' WHERE wid=");
  buf.Printf(_T("%d;"), wid);
  query += buf;

  lWorkers->Query(query);
  lWorkers->SelectAll();
}

void proj_testFrame::OnbWorkDeleteClick(wxCommandEvent& event)
{
  wxString buf;
  wxString query = _T("SELECT * FROM workers WHERE wid=");
  buf.Printf(_T("%d;"), wid);
  query += buf;
  try {
    wxString question = _("Are you sure you want to delete worker:\n");
    DatabaseResultSet * q = db.RunQueryWithResults(query);
    if (q->Next())
    {
      question += q->GetResultString(_("surname")) + _(" ");
      question += q->GetResultString(_("name"));
      question += _(" from ") + q->GetResultString(_("city")) + _(" ?");
      db.CloseResultSet(q);
    } else return;

    int result = wxMessageBox(question, _T("question"), wxYES_NO);
    if (result == wxNO) return;

    query = _T("DELETE FROM workers WHERE wid=");
    buf.Printf(_T("%d;"), wid);
    query += buf;

    lWorkers->Query(query);
    lWorkers->SelectAll();

    // Not needed, since we CASCADE foreign keys
    // query = _T("DELETE FROM assignments WHERE userid=");
    // buf.Printf(_T("%d"), m_userid);
    // query += buf + _T(";");
    // UpdateQuery(query);
  }
  catch (DatabaseLayerException & e)
  {
      wxMessageBox(e.GetErrorMessage(), _("Exc OnbWorkDeleteClick"));
  }
}
