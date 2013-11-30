#include "test.h"
#include <iostream>

using namespace std;

#define NUM_ITEMS 10

#ifndef __WXMSW__
#include "small1.xpm"
#endif




Ident::Ident(const wxString& title)
    : //wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(200, 150))
    wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 650))
{

  m_book = new wxBookCtrl(this, ID_BOOK);
  wxPanel *panel = new wxPanel(m_book);  
  
  m_imageListSmall = new wxImageList(16, 16, true);
#ifdef __WXMSW__
  SetIcon( wxIcon(_T("iconsmall"), wxBITMAP_TYPE_ICO_RESOURCE) );
  m_imageListSmall->Add( wxIcon(_T("iconsmall"), wxBITMAP_TYPE_ICO_RESOURCE) );
#else
  //SetIcon( wxIcon(small1_xpm) ); // makes errors with SQLite3: 21:SQLITE_MISUSE[21]: library routine called  out of sequence, ie. closes the database without setting a proper bool IsOpen() flag
  m_imageListSmall->Add( wxIcon( small1_xpm) );
#endif
    

  
  //
  // TOOLBAR
  //
  //wxBitmap exit(wxT("wxwin.png"));
#ifdef __WXMSW__
  wxBitmap exit( wxIcon(_T("iconsmall"), wxBITMAP_TYPE_ICO_RESOURCE)); 
#else
  wxBitmap exit( wxT("wxwin.png") );
#endif
/*
  wxToolBar *toolBar = CreateToolBar();
  toolBar->AddTool(wxID_NEW, exit, wxT("New file"));
  toolBar->Realize();
*/


  //
  // SIZERS
  //
  wxButton * quit = new wxButton(panel, wxID_EXIT);
  
  
  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
  
  wxSQLite3Database db;
  try
  {
    
    wxSQLite3Database::InitializeSQLite();
    db.Open(DBFILE);
    //db.ExecuteUpdate("ALTER TABLE projects ADD revenue INTEGER;"); 
   // if (!m_db.TableExists(_T("workers")));  // doesn't work anyway
    {
      db.ExecuteUpdate(_T("CREATE TABLE IF NOT EXISTS workers (surname TEXT, name TEXT, city TEXT, salary INTEGER NOT NULL, usrerid INTEGER PRIMARY KEY)"));
      db.ExecuteQuery(_T("CREATE TABLE IF NOT EXISTS projects (projectid INTEGER PRIMARY KEY, projectname TEXT NOT NULL, revenue INTEGER NOT NULL);"));
      db.ExecuteQuery(_T("CREATE TABLE IF NOT EXISTS assignments (userid INTEGER NOT NULL, projectid INTEGER NOT NULL, FOREIGN KEY (userid) REFERENCES workers(userid), FOREIGN KEY (projectid) REFERENCES projects(projectid));"));
      db.Close();
    }
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "At Ident::Ident " << e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
  
  //wxStaticBox * statBox = new wxStaticBox(panel, wxID_ANY, _T("&Box around combobox"), wxPoint(30, 39), wxSize(150, 100));
  
  m_listCtrl = new MyListCtrl(panel, ID_LISTBOX, wxPoint(-1, -1), wxSize(-1, -1), wxLC_REPORT, &db);
  InitWithReportItems();
  
 // m_listCtrl->InsertItemInReportView(0);
 // m_listCtrl->Append(wxT("First line"));
  //m_listCtrl->Append(wxT("Second line"));
  
  wxBoxSizer *hbox0 = new wxBoxSizer(wxHORIZONTAL);
  hbox0->Add(m_listCtrl, 1, wxEXPAND | wxALL, 10);
  vbox->Add(hbox0, 1, wxEXPAND);
  
  
					      
  m_textSurname = new wxTextCtrl(panel,   ID_ED_SURNAME);
  m_textName =    new wxTextCtrl(panel,   ID_ED_NAME);
  m_textCity =    new wxTextCtrl(panel,   ID_ED_CITY);
  m_textSalary =  new wxTextCtrl(panel,   ID_ED_SALARY);
  
  m_textResult =  new wxTextCtrl(panel,   ID_ED_RESULT);
  
  m_textLess =    new wxTextCtrl(panel,   ID_ED_LESS);
  m_textMore =   new wxTextCtrl(panel,   ID_ED_MORE);
  
  wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
  hbox1->Add(new wxStaticText(panel, wxID_STATIC, wxT("Surname")), 1, 
             wxLEFT | wxRIGHT | wxUP, 10); 
  hbox1->Add(new wxStaticText(panel, wxID_STATIC, wxT("Name")), 1, 
	     wxLEFT | wxRIGHT | wxUP, 10); 
  hbox1->Add(new wxStaticText(panel, wxID_STATIC, wxT("City")), 1, 
	     wxLEFT | wxRIGHT | wxUP, 10); 
  hbox1->Add(new wxStaticText(panel, wxID_STATIC, wxT("Salary")), 1, 
	     wxLEFT | wxRIGHT | wxUP, 10); 
  
  wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
  hbox2->Add(m_textSurname, 1, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxDOWN, 10); 
  hbox2->Add(m_textName, 1, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT| wxDOWN, 10); 
  hbox2->Add(m_textCity, 1, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT| wxDOWN, 10); 
  hbox2->Add(m_textSalary, 1, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT| wxDOWN, 10); 
  
  wxStaticBox *boxFields = new wxStaticBox(panel, wxID_ANY, _T("Fields"));
  wxSizer *sizerFields = new wxStaticBoxSizer(boxFields, wxVERTICAL);
  sizerFields->Add(hbox1, 0, wxEXPAND); 
  sizerFields->Add(hbox2, 0, wxEXPAND); 
  vbox->Add(sizerFields, 0, wxEXPAND| wxLEFT | wxRIGHT, 10);
  
  vbox->Add(-1, 10);

  
  wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);
  hbox3->Add(new wxButton(panel, wxID_NEW), 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  hbox3->Add(new wxButton(panel, wxID_SAVE), 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  hbox3->Add(new wxButton(panel, wxID_DELETE), 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

  wxBoxSizer *hbox4 = new wxBoxSizer(wxHORIZONTAL);  
  hbox4->Add(new wxButton(panel, ID_SEARCH_AND, _T("Search AND")), 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  hbox4->Add(new wxButton(panel, ID_SEARCH_OR,  _T("Search OR")), 1,  wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  hbox4->Add(new wxButton(panel, ID_SELECT_ALL, _T("Select All")), 1,  wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  
  wxStaticBox *boxData = new wxStaticBox(panel, wxID_ANY, _T("Data manipulation"));
  wxSizer *sizerData = new wxStaticBoxSizer(boxData, wxVERTICAL);
  sizerData->Add(hbox3, 0, wxEXPAND);
  wxStaticBox *boxSearch = new wxStaticBox(panel, wxID_ANY, _T("Basic search"));
  wxSizer *sizerSearch = new wxStaticBoxSizer(boxSearch, wxVERTICAL);
  sizerSearch->Add(hbox4, 0, wxEXPAND);
  
  wxGridSizer * gs2 = new wxGridSizer(1, 2, 5, 50);
  gs2->Add(sizerData, 1, wxEXPAND | wxLEFT , 10);
  gs2->Add(sizerSearch, 1, wxEXPAND | wxRIGHT, 10);
  vbox->Add(gs2, 0, wxEXPAND );
  
  vbox->Add(-1, 10);
  
  wxGridSizer * gs3Sub1 = new wxGridSizer(3, 2, 10, 0);
  gs3Sub1->Add(new wxButton(panel, ID_SUM,     _T("Sum")), 1, wxALIGN_CENTER_HORIZONTAL );
  gs3Sub1->Add(new wxButton(panel, ID_MIN,     _T("Min")), 1, wxALIGN_CENTER_HORIZONTAL );
  gs3Sub1->Add(new wxButton(panel, ID_AVERAGE, _T("Average")), 1, wxALIGN_CENTER_HORIZONTAL );
  gs3Sub1->Add(new wxButton(panel, ID_MAX,     _T("Max")), 1, wxALIGN_CENTER_HORIZONTAL );
  gs3Sub1->Add(new wxButton(panel, ID_COUNT,   _T("Count")), 1, wxALIGN_CENTER_HORIZONTAL);
  gs3Sub1->Add(m_textResult,                                  1, wxALIGN_CENTER_HORIZONTAL);
  
  wxGridSizer * gs3Sub2 = new wxGridSizer(3, 2, 10, 0);
  gs3Sub2->Add(new wxButton(panel, ID_LESS, _T("Less than")),  1, wxALIGN_CENTER_HORIZONTAL);
  gs3Sub2->Add(m_textLess,                            1, wxALIGN_CENTER_HORIZONTAL);
  gs3Sub2->Add(new wxButton(panel, ID_MORE, _T("More than")), 1, wxALIGN_CENTER_HORIZONTAL );
  gs3Sub2->Add(m_textMore,                            1, wxALIGN_CENTER_HORIZONTAL );
  gs3Sub2->Add(new wxButton(panel, ID_BETWEEN, _T("Between")), 1, wxALIGN_CENTER_HORIZONTAL);
  gs3Sub2->Add(new wxButton(panel, ID_NOT_BETWEEN, _T("Not between")), 1, wxALIGN_CENTER_HORIZONTAL);
  
  wxStaticBox *boxFunctions = new wxStaticBox(panel, wxID_ANY, _T("Salary functions"));
  wxSizer *sizerFunctions = new wxStaticBoxSizer(boxFunctions, wxVERTICAL);
  sizerFunctions->Add(gs3Sub1, 0, wxEXPAND | wxALL, 10);
  
  wxStaticBox *boxRanges = new wxStaticBox(panel, wxID_ANY, _T("Salary ranges"));
  wxSizer *sizerRanges = new wxStaticBoxSizer(boxRanges, wxVERTICAL);
  sizerRanges->Add(gs3Sub2, 0, wxEXPAND | wxALL, 10);
  
  wxGridSizer * gs3 = new wxGridSizer(1, 2, 10, 80);
  gs3->Add(sizerFunctions, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
  gs3->Add(sizerRanges, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
  vbox->Add(gs3, 0, wxEXPAND );
  
  

  

  
  vbox->Add(-1, 10);
  
  wxBoxSizer *hbox5 = new wxBoxSizer(wxHORIZONTAL);
  hbox5->Add(quit);

  vbox->Add(hbox5, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
  
  panel->SetSizer(vbox);

  //
  // BUTTONS
  //
  
/*
  wxGridSizer *grid = new wxGridSizer(3, 3);

  grid->Add(new wxButton(panel, wxID_CANCEL), 0, wxLEFT | wxTOP, 9);
  grid->Add(new wxButton(panel, wxID_DELETE), 0, wxTOP, 9);
  grid->Add(new wxButton(panel, wxID_SAVE), 0, wxRIGHT| wxTOP , 9);
  grid->Add(quit, 0, wxLEFT, 9);
  grid->Add(quit2, 0, 0, 9);
  grid->Add(new wxButton(panel, wxID_STOP), 0, wxLEFT, 9);
  grid->Add(new wxButton(panel, wxID_NEW));

  panel->SetSizer(grid);
*/  
  
  Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnQuit));
  
  Connect(wxID_NEW, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnNew));
  Connect(wxID_SAVE, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnUpdate));
  Connect(wxID_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnDelete));

  Connect(ID_SEARCH_AND, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnSearchAND));
  Connect(ID_SEARCH_OR, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnSearchOR));
  Connect(ID_SELECT_ALL, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnSelectAll));
  
  Connect(ID_SUM, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnFunctions));
  Connect(ID_AVERAGE, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnFunctions));
  Connect(ID_COUNT, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnFunctions));
  Connect(ID_MIN, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnFunctions));
  Connect(ID_MAX, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnFunctions));
  
  Connect(ID_LESS, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnMoreLess));
  Connect(ID_MORE, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnMoreLess));
  Connect(ID_BETWEEN, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnBetween));
  Connect(ID_NOT_BETWEEN, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(Ident::OnBetween));
  
  // 
  // END BUTTONS
  //
  
 
  
  //
  // MENUS
  //
  wxMenuBar * menuBar = new wxMenuBar;
  
  wxMenu * fileMenu = new wxMenu;
  fileMenu->Append(wxID_NEW, wxT("&New\tCtrl+N"));
  fileMenu->Append(wxID_OPEN, wxT("&Open\tCtrl+O"));
  fileMenu->Append(wxID_SAVE, wxT("&Save\tCtrl+S"));
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_PRINT, wxT("&Print\tCtrl+P"));
  fileMenu->AppendSeparator();

  wxMenu * importMenu = new wxMenu;
  importMenu->Append(ID_MNU_IMPORT_NEWS, wxT("Import newsfeed list..."));
  importMenu->Append(ID_MNU_IMPORT_BOOK, wxT("Import bookmarks..."));
  importMenu->Append(ID_MNU_IMPORT_MAIL, wxT("Import mail..."));

  fileMenu->AppendSubMenu(importMenu, wxT("I&mport\tCtrl+M"));
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, wxT("&Exit\tCtrl+Q"));
  menuBar->Append(fileMenu, wxT("&File"));
  
  wxMenu * listMenu = new wxMenu;
  listMenu->Append(ID_MNU_WIDTH_DEF, wxT("Default columns' width"));
  listMenu->Append(ID_MNU_WIDTH_AUTO, wxT("Automatic columns' width"));
  listMenu->AppendSeparator();
  listMenu->AppendCheckItem(ID_LIST_TOGGLE_LINES, _T("Disable &lines\tCtrl-L"));
  menuBar->Append(listMenu, wxT("&View"));
  
  wxMenu * helpMenu = new wxMenu;
  helpMenu->Append(wxID_HELP, wxT("&Help\tF1"));
  helpMenu->Append(wxID_ABOUT, wxT("&About\tCtrl+A"));  
  menuBar->Append(helpMenu, wxT("&Help"));
  
  SetMenuBar(menuBar);

  Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, 
          wxCommandEventHandler(Ident::OnNew));
  Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, 
          wxCommandEventHandler(Ident::OnQuit));
  Connect(ID_MNU_WIDTH_DEF, wxEVT_COMMAND_MENU_SELECTED, 
          wxCommandEventHandler(Ident::OnDefaultColumnWidth));
  Connect(ID_MNU_WIDTH_AUTO, wxEVT_COMMAND_MENU_SELECTED, 
          wxCommandEventHandler(Ident::OnAutoColumnWidth));
  Connect(ID_LIST_TOGGLE_LINES, wxEVT_COMMAND_MENU_SELECTED, 
          wxCommandEventHandler(Ident::OnToggleLines));
  Connect(wxID_HELP, wxEVT_COMMAND_MENU_SELECTED, 
          wxCommandEventHandler(Ident::OnHelp));
  Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, 
          wxCommandEventHandler(Ident::OnAbout));
  
  m_book->AddPage(panel, _T("Workers"), true);
   
  panel = new wxPanel(m_book);
  
  
  vbox = new wxBoxSizer(wxVERTICAL);
  
  m_projListCtrl = new ProjListCtrl(panel, ID_PROJ_LISTBOX, wxPoint(-1, -1), wxSize(-1, -1), wxLC_REPORT, &db);
  InitProjWithReportItems();
  
  hbox0 = new wxBoxSizer(wxHORIZONTAL);
  hbox0->Add(m_projListCtrl, 1, wxEXPAND | wxALL, 10);
  vbox->Add(hbox0, 1, wxEXPAND);
  
  hbox1 = new wxBoxSizer(wxHORIZONTAL);
  hbox1->Add(new wxButton(panel, ID_NEW, _T("New")), 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  hbox1->Add(new wxButton(panel, ID_SAVE, _T("Save")), 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  hbox1->Add(new wxButton(panel, ID_DELETE, _T("Delete")), 1, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);
  
  boxData = new wxStaticBox(panel, wxID_ANY, _T("Data manipulation"));
  sizerData = new wxStaticBoxSizer(boxData, wxVERTICAL);
  sizerData->Add(hbox1, 0, wxEXPAND); 
					      
  m_textProjName = new wxTextCtrl(panel,   ID_ED_PROJNAME);
  m_textProjRev =  new wxTextCtrl(panel,   ID_ED_PROJREV);
  
  hbox1 = new wxBoxSizer(wxHORIZONTAL);
  hbox1->Add(new wxStaticText(panel, wxID_STATIC, wxT("Name")), 1, 
             wxLEFT | wxRIGHT | wxUP, 10); 
  hbox1->Add(new wxStaticText(panel, wxID_STATIC, wxT("Revenue")), 1, 
             wxLEFT | wxRIGHT | wxUP, 10); 
  
  hbox2 = new wxBoxSizer(wxHORIZONTAL);
  hbox2->Add(m_textProjName , 1, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT | wxDOWN, 10); 
  hbox2->Add(m_textProjRev, 1, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT| wxDOWN, 10); 
  
  boxFields = new wxStaticBox(panel, wxID_ANY, _T("Fields"));
  sizerFields = new wxStaticBoxSizer(boxFields, wxVERTICAL);
  sizerFields->Add(hbox1, 0, wxEXPAND); 
  sizerFields->Add(hbox2, 0, wxEXPAND); 
  
  wxGridSizer * gs1 = new wxGridSizer(1, 2, 5, 30);
  gs1->Add(sizerData, 1, wxEXPAND);
  gs1->Add(sizerFields, 1, wxEXPAND);
  
  vbox->Add(gs1, 0, wxEXPAND| wxLEFT | wxRIGHT, 10);
  
  vbox->Add(-1, 10);
  
  hbox5 = new wxBoxSizer(wxHORIZONTAL);
  hbox5->Add(new wxButton(panel, wxID_EXIT));

  vbox->Add(hbox5, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
 
  panel->SetSizer(vbox);
  m_book->AddPage(panel, _T("Projects"));
  
  
  Connect(ID_NEW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Ident::OnProjNew));
  Connect(ID_SAVE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Ident::OnProjUpdate));
  Connect(ID_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Ident::OnProjDelete));
  
  Connect(ID_BOOK, wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGING, wxCommandEventHandler(Ident::OnBookChange));
  
  Centre();
}

Ident::~Ident()
{
  delete m_imageListSmall;
  
  try 
  {
    wxSQLite3Database::ShutdownSQLite();
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
}

void Ident::OnProjNew(wxCommandEvent & WXUNUSED(event))
{
  m_projListCtrl->Add();
}

void Ident::OnProjUpdate(wxCommandEvent & WXUNUSED(event))
{
  m_projListCtrl->Update();
}

void Ident::OnProjDelete(wxCommandEvent & WXUNUSED(event))
{
  m_projListCtrl->Delete();
}

void Ident::OnQuit(wxCommandEvent & WXUNUSED(event))
{
 Close(true); 
}

void Ident::OnNew(wxCommandEvent & WXUNUSED(event))
{
  m_listCtrl->Add();
}

void Ident::OnUpdate(wxCommandEvent & WXUNUSED(event))
{
  m_listCtrl->Update();
}
    
void Ident::OnDelete(wxCommandEvent & WXUNUSED(event))
{
  m_listCtrl->Delete();
}

void Ident::OnSearchAND(wxCommandEvent & WXUNUSED(event))
{
  m_listCtrl->SearchFromFields(true);
}

void Ident::OnSearchOR(wxCommandEvent & WXUNUSED(event))
{
  m_listCtrl->SearchFromFields(false);
}
  
void Ident::OnSelectAll(wxCommandEvent & WXUNUSED(event))
{
  m_listCtrl->SelectAll();
}

void Ident::OnFunctions(wxCommandEvent & event)
{
  m_listCtrl->Functions(event);
}

void Ident::OnMoreLess(wxCommandEvent & event)
{
  m_listCtrl->MoreLess(event);
}

void Ident::OnBetween(wxCommandEvent & event)
{
  m_listCtrl->Between(event);
}



 
void Ident::InitWithReportItems()
{
  m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    // note that under MSW for SetColumnWidth() to work we need to create the
    // items with images initially even if we specify dummy image id
  wxListItem itemCol;
  itemCol.SetText(_T("Surname"));
  itemCol.SetImage(-1);
  m_listCtrl->InsertColumn(0, itemCol);

  itemCol.SetText(_T("Name"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  m_listCtrl->InsertColumn(1, itemCol);

  itemCol.SetText(_T("City"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  m_listCtrl->InsertColumn(2, itemCol);
  
  itemCol.SetText(_T("Salary"));
  itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
  m_listCtrl->InsertColumn(3, itemCol);

    // to speed up inserting we hide the control temporarily
 // m_listCtrl->Hide();

 // wxStopWatch sw;

  m_listCtrl->SelectAll();
  /*
  for ( int i = 0; i < NUM_ITEMS; i++ )
  {
    m_listCtrl->InsertItemInReportView(i);
  }
*/
  //m_logWindow->WriteText(wxString::Format(_T("%d items inserted in %ldms\n"),                         NUM_ITEMS, sw.Time()));
 // m_listCtrl->Show();

  /*
    // we leave all mask fields to 0 and only change the colour
  wxListItem item;
  item.m_itemId = 0;
  item.SetTextColour(*wxRED);
  m_listCtrl->SetItem( item );

  item.m_itemId = 2;
  item.SetTextColour(*wxGREEN);
  m_listCtrl->SetItem( item );
  item.m_itemId = 4;
  item.SetTextColour(*wxLIGHT_GREY);
  item.SetFont(*wxITALIC_FONT);
  item.SetBackgroundColour(*wxRED);
  m_listCtrl->SetItem( item );
  
  

  m_listCtrl->SetColumnWidth( 0, wxLIST_AUTOSIZE );
  m_listCtrl->SetColumnWidth( 1, wxLIST_AUTOSIZE );
  m_listCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE );
  m_listCtrl->SetColumnWidth( 3, wxLIST_AUTOSIZE );
  */
  m_listCtrl->SetTextColour(*wxBLUE);
  m_listCtrl->SetSingleStyle(wxLC_HRULES | wxLC_VRULES, true);
  int colWidth = 170;  
  m_listCtrl->SetColumnWidth( 0, colWidth );
  m_listCtrl->SetColumnWidth( 1, colWidth );
  m_listCtrl->SetColumnWidth( 2, colWidth );
  m_listCtrl->SetColumnWidth( 3, colWidth );
  /*
    // Set images in columns
  m_listCtrl->SetItemColumnImage(1, 1, 0);

  wxListItem info;
  info.SetImage(0);
  info.SetId(3);
  info.SetColumn(2);
  m_listCtrl->SetItem(info);

    // test SetItemFont too
  m_listCtrl->SetItemFont(0, *wxITALIC_FONT);
  */
  
}

void Ident::InitProjWithReportItems()
{
  m_projListCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    // note that under MSW for SetColumnWidth() to work we need to create the
    // items with images initially even if we specify dummy image id
  wxListItem itemCol;
  itemCol.SetText(_T("Project"));
  itemCol.SetImage(-1);
  m_projListCtrl->InsertColumn(0, itemCol);

  itemCol.SetText(_T("Revenue"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  m_projListCtrl->InsertColumn(1, itemCol);

  itemCol.SetText(_T("Taken by"));
  itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
  m_projListCtrl->InsertColumn(2, itemCol);
  
  itemCol.SetText(_T("Revenue per worker"));
  itemCol.SetAlign(wxLIST_FORMAT_RIGHT);
  m_projListCtrl->InsertColumn(3, itemCol);

  m_projListCtrl->SelectAll();

  m_projListCtrl->SetTextColour(*wxRED);
  m_projListCtrl->SetSingleStyle(wxLC_HRULES | wxLC_VRULES, true); 
  //m_projListCtrl->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    
  m_projListCtrl->SetColumnWidth( 0, m_colWidth );
  m_projListCtrl->SetColumnWidth( 1, m_colWidth );
  m_projListCtrl->SetColumnWidth( 2, m_colWidth );
  m_projListCtrl->SetColumnWidth( 3, m_colWidth );
}


        
void Ident::OnDefaultColumnWidth(wxCommandEvent& WXUNUSED(event))
{
  m_listCtrl->SetColumnWidth( 0, m_colWidth );
  m_listCtrl->SetColumnWidth( 1, m_colWidth );
  m_listCtrl->SetColumnWidth( 2, m_colWidth );
  m_listCtrl->SetColumnWidth( 3, m_colWidth );
  
  m_projListCtrl->SetColumnWidth( 0, m_colWidth );
  m_projListCtrl->SetColumnWidth( 1, m_colWidth );
  m_projListCtrl->SetColumnWidth( 2, m_colWidth );
  m_projListCtrl->SetColumnWidth( 3, m_colWidth );
}

void Ident::OnAutoColumnWidth(wxCommandEvent& WXUNUSED(event))
{
  m_listCtrl->SetColumnWidth( 0, wxLIST_AUTOSIZE );
  m_listCtrl->SetColumnWidth( 1, wxLIST_AUTOSIZE );
  m_listCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE );
  m_listCtrl->SetColumnWidth( 3, wxLIST_AUTOSIZE );
  
  m_projListCtrl->SetColumnWidth( 0, wxLIST_AUTOSIZE );
  m_projListCtrl->SetColumnWidth( 1, wxLIST_AUTOSIZE );
  m_projListCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE );
  m_projListCtrl->SetColumnWidth( 3, wxLIST_AUTOSIZE );
}

void Ident::OnToggleLines(wxCommandEvent& event)
{
  m_listCtrl->SetSingleStyle(wxLC_HRULES | wxLC_VRULES, !event.IsChecked());
  m_projListCtrl->SetSingleStyle(wxLC_HRULES | wxLC_VRULES, !event.IsChecked());
}

void Ident::OnHelp(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_T("Data manipulation - New adds a worker specified by Fields, Save modifies selected worker, Delete deletes selected worker\nDouble click on a worker or on a project or select it and press Enter to assign one to another. On the Projects page you'll see how many workers have been assigned to a given project"), 
               _T("Help"),
               wxOK | wxICON_INFORMATION,
               this);
}

void Ident::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_T("This is a database sample\n\nSee BSD-licence for copyright"), 
               _T("About"),
               wxOK | wxICON_INFORMATION,
               this);
}
void Ident::OnBookChange(wxCommandEvent & WXUNUSED(event))
{
  m_projListCtrl->SelectAll();
}
