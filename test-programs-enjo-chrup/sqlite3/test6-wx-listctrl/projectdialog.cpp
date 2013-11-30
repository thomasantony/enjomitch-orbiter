#include <iostream>
#include "projectdialog.h"
#include "globals.h"
#include "projListCtrl.h"

using namespace std;

ProjectDialog::ProjectDialog(wxWindow * parent, const wxString & title, wxSQLite3Database* db, long projid)
  : wxDialog(parent, -1, title, wxDefaultPosition, wxSize(200, 280))
{
  m_parent = parent;
  //wxPanel *panel = new wxPanel(this, -1);
  m_db = db;
  if (!m_db->IsOpen())  m_db->Open(DBFILE); 
  m_projid = projid;
  
  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
 // vbox->Add(panel, 1);
  
  
 // wxStaticBox *boxAss = new wxStaticBox(this, wxID_ANY, _T("Assignments"));
 // wxSizer *sizerAss = new wxStaticBoxSizer(boxAss, wxVERTICAL);
  m_asslbox = new wxCheckListBox(this, ID_ASSIGNMENT_LISTBOX, wxDefaultPosition, wxDefaultSize);
 // sizerAss->Add(m_asslbox, 0, wxEXPAND);

  hbox->Add(new wxButton(this, wxID_CANCEL), 1, wxRIGHT, 10);
  hbox->Add(new wxButton(this, wxID_OK), 1, wxLEFT, 10);

  vbox->Add(m_asslbox, 1, wxEXPAND);
  vbox->Add(hbox, 0, wxEXPAND);

  SetSizer(vbox);

  InitCheckListbox();
  
  
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, 
          wxCommandEventHandler(ProjectDialog::OnOK));
  Centre();
  
  ShowModal();

  Destroy(); 
  
}


void ProjectDialog::InitCheckListbox()
{
  wxSQLite3ResultSet q, test;
  
  try
  {
    if (!m_db->IsOpen())  m_db->Open(DBFILE); 
  /*
    m_db->ExecuteUpdate(_T("INSERT INTO projects VALUES (NULL, 'Databases');"));
    m_db->ExecuteUpdate(_T("INSERT INTO projects VALUES (NULL, 'Lazzers');"));
    m_db->ExecuteUpdate(_T("INSERT INTO projects VALUES (NULL, 'Hydroponics');"));
    m_db->ExecuteUpdate(_T("INSERT INTO projects VALUES (NULL, 'Plasma');"));
    m_db->ExecuteUpdate(_T("INSERT INTO projects VALUES (NULL, 'Shock rifles');"));
    m_db->ExecuteUpdate(_T("INSERT INTO projects VALUES (NULL, 'Psi amplifiers');"));
  */
    // Search for workers assigned to this project
    wxSQLite3ResultSet qAss;
    
    wxString query;
    query.Printf(_T("SELECT userid FROM assignments WHERE projectid=%d;"), m_projid);
    qAss = m_db->ExecuteQuery(query);
    m_arrAssProj.Clear();

    while (qAss.NextRow())
    {
      m_arrAssProj.Add(qAss.GetInt(0));
    }
    

    
    q = m_db->ExecuteQuery(_T("SELECT userid, surname, name FROM workers;"));
  
    m_arrUids.Clear();
    wxArrayString items;
    
    while (q.NextRow())
    {
      m_arrUids.Add(q.GetInt(0));
      items.Add(q.GetString(1) + _T(" ") + q.GetString(2));
    }
    
    m_asslbox->InsertItems(items, 0);
    
    for (size_t i = 0; i < m_arrUids.Count(); i++)
    {
      bool found = false;
      for (size_t j = 0; j < m_arrAssProj.Count(); j++)
      {
        // if this project from the whole list has already been assigned to this worker, check the item in checklistbox
        if (m_arrUids.Item(i) == m_arrAssProj.Item(j))
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
    cerr << "ProjectDialog::InitCheckListbox() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
  
}

void ProjectDialog::OnOK(wxCommandEvent & WXUNUSED(event))
{
  wxBell();
  try
  {

    // and check if the items are checked
    for (size_t i = 0; i < m_arrUids.Count(); i++)
    {
        bool found = false;
        // scan for projects belonging to this user
        for (size_t j = 0; j < m_arrAssProj.Count(); j++)
        {
          if (m_arrUids.Item(i) == m_arrAssProj.Item(j))
          {
           found = true;
           break;  
          }
        }
        wxString query;
        // if item checked, and no previous entries found (newly ckecked), insert a new assignment
        if (m_asslbox->IsChecked(i) && !found)
        {
          query.Printf(_T("INSERT INTO assignments VALUES (%d, %d)"), m_arrUids.Item(i), m_projid);
          m_db->ExecuteUpdate(query);
        }
        else if (!m_asslbox->IsChecked(i) && found)
        {
          query.Printf(_T("DELETE FROM assignments WHERE userid=%d AND projectid=%d"),  m_arrUids.Item(i), m_projid);
          m_db->ExecuteUpdate(query);
        }
    }
    // update the Project listctl
    ProjListCtrl * projListCtrl = (ProjListCtrl *) m_parent;
    projListCtrl->SelectAll();
  }
  
  catch (wxSQLite3Exception& e)
  {
    cerr << "ProjectDialog::InitCheckListbox() " <<e.GetErrorCode() << ":" << (const char*)(e.GetMessage().mb_str()) << endl;
  }
  Destroy();
}