#include <iostream>
#include "customdialog.h"
#include "globals.h"

#ifndef __WXMSW__
#include "small1.xpm"
#endif

using namespace std;

CustomDialog::CustomDialog(wxWindow * parent, const wxString & title, wxSQLite3Database* db, long userid)
  : wxDialog(parent, -1, title, wxDefaultPosition, wxSize(200, 280))
{
#ifndef __WXMSW__
  SetIcon( wxIcon(small1_xpm) );
#endif
  m_parent = parent;
  //wxPanel *panel = new wxPanel(this, -1);
  m_db = db;
  if (!m_db->IsOpen())  m_db->Open(DBFILE); 
  m_userid = userid;
  
  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
 // vbox->Add(panel, 1);
  
  
 // wxStaticBox *boxAss = new wxStaticBox(this, wxID_ANY, _T("Assignments"));
 // wxSizer *sizerAss = new wxStaticBoxSizer(boxAss, wxVERTICAL);
  m_asslbox = new wxCheckListBox(this, ID_ASSIGNMENT_LISTBOX, wxDefaultPosition, wxDefaultSize);
  //wxCheckBox * m_assbox = new wxCheckBox(this, ID_ASSIGNMENT_LISTBOX, _T("check"), wxDefaultPosition, wxDefaultSize,  wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);
 // sizerAss->Add(m_asslbox, 0, wxEXPAND);

  hbox->Add(new wxButton(this, wxID_CANCEL), 1, wxRIGHT, 10);
  hbox->Add(new wxButton(this, wxID_OK), 1, wxLEFT, 10);
  

  //vbox->Add(m_assbox, 1, wxEXPAND);
  vbox->Add(m_asslbox, 1, wxEXPAND);
  vbox->Add(hbox, 0, wxEXPAND);

  SetSizer(vbox);

  InitCheckListbox();
  
  
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(CustomDialog::OnOK));
  Centre();
  
  ShowModal();

  Destroy(); 
  
}


void CustomDialog::InitCheckListbox()
{
  wxSQLite3ResultSet q, test;
  
  try
  {
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
    //m_db->Open(DBFILE); // interferes with Ident::SetIcon



    // Search for projects assigned to this worker
    wxSQLite3ResultSet qAss;
    
    wxString query;
    query.Printf(_T("SELECT projectid FROM assignments WHERE userid=%d;"), m_userid);
    qAss = m_db->ExecuteQuery(query);
    m_arrAssProj.Clear();

    while (qAss.NextRow())
    {
      m_arrAssProj.Add(qAss.GetInt(0));
    }
    

    
    q = m_db->ExecuteQuery(_T("SELECT projectid, projectname FROM projects;"));
  
    m_arrPids.Clear();
    wxArrayString items;
    
    while (q.NextRow())
    {
      m_arrPids.Add(q.GetInt(0));
      items.Add(q.GetString(1));
    }
    
    m_asslbox->InsertItems(items, 0);
    
    for (size_t i = 0; i < m_arrPids.Count(); i++)
    {
      bool found = false;
      for (size_t j = 0; j < m_arrAssProj.Count(); j++)
      {
        // if this project from the whole list has already been assigned to this worker, check the item in checklistbox
        if (m_arrPids.Item(i) == m_arrAssProj.Item(j))
        {
          found = true;
          break; 
        }
      }
      if (found)
        m_asslbox->Check(i);
    }
  }
  catch (wxSQLite3Exception& e)
  {
    cerr << "CustomDialog::InitCheckListbox() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
  
}

void CustomDialog::OnOK(wxCommandEvent & WXUNUSED(event))
{
  wxBell();
  try
  {

    // and check if the items are checked
    for (size_t i = 0; i < m_arrPids.Count(); i++)
    {
        bool found = false;
        // scan for projects belonging to this user
        for (size_t j = 0; j < m_arrAssProj.Count(); j++)
        {
          if (m_arrPids.Item(i) == m_arrAssProj.Item(j))
          {
           found = true;
           break;  
          }
        }
        wxString query;
        // if item checked, and no previous entries found (newly ckecked), insert a new assignment
        if (m_asslbox->IsChecked(i) && !found)
        {
          query.Printf(_T("INSERT INTO assignments VALUES (%d, %d)"), m_userid, m_arrPids.Item(i));
          m_db->ExecuteUpdate(query);
        }
        else if (!m_asslbox->IsChecked(i) && found)
        {
          query.Printf(_T("DELETE FROM assignments WHERE userid=%d AND projectid=%d"), m_userid, m_arrPids.Item(i));
          m_db->ExecuteUpdate(query);
        }
    }
  }
  
  catch (wxSQLite3Exception& e)
  {
    cerr << "CustomDialog::InitCheckListbox() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
  Destroy();
}
