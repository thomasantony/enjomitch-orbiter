#include "AssDialog.h"
#include "proj_testMain.h"
#include <wx/databaselayer/DatabaseLayerException.h>

#ifndef __WXMSW__
  #include "small1.xpm"
#endif

const long AssDialog::ID_ASSIGNMENT_LISTBOX = wxNewId();

AssDialog::AssDialog(wxWindow * parent, const wxString & title, MysqlDatabaseLayer* db, long id)
: wxDialog(parent, -1, title, wxDefaultPosition, wxSize(200, 280))
{
#ifndef __WXMSW__
  SetIcon( wxIcon(small1_xpm) );
#endif
  m_parent = parent;
  //wxPanel *panel = new wxPanel(this, -1);
  m_db = db;
  m_id = id; // can be wid or pid

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
}

void AssDialog::OnOK(wxCommandEvent & WXUNUSED(event))
{
  wxBell();
  try
  {

    // and check if the items are checked
    for (size_t i = 0; i < m_arrIds.Count(); i++)
    {
        bool found = false;
        // scan for projects belonging to this user
        for (size_t j = 0; j < m_arrAssProj.Count(); j++)
        {
          if (m_arrIds.Item(i) == m_arrAssProj.Item(j))
          {
           found = true;
           break;
          }
        }
        wxString query;
        // if item checked, and no previous entries found (newly ckecked), insert a new assignment
        if (m_asslbox->IsChecked(i) && !found)
        {
          //query.Printf(_T("INSERT INTO assignments VALUES (%d, %d)"), m_wid, m_arrIds.Item(i));
          query = _T("INSERT INTO assignments (") + m_str1stId + _T(", ") + m_str2ndId + _T(") ");
          query += wxString::Format( _T("VALUES (%d, %d)"), m_id, m_arrIds.Item(i) );
          m_db->ExecuteUpdate(query);
        }
        else if (!m_asslbox->IsChecked(i) && found)
        {
          //query.Printf(_T("DELETE FROM assignments WHERE wid=%d AND pid=%d;"), m_wid, m_arrIds.Item(i));
          query = _T("DELETE FROM assignments WHERE ") + m_str1stId;
          query += wxString::Format( _T(" = %d AND "), m_id ) + m_str2ndId;
          query += wxString::Format( _T(" = %d;"), m_arrIds.Item(i) );
          m_db->ExecuteUpdate(query);
        }
    }
    proj_testFrame * frame = (proj_testFrame *) m_parent;
    frame->SelectAllEverywhere();
  }
  catch (DatabaseLayerException & e)
  {
      wxMessageBox(e.GetErrorMessage(), _("Exc WorkAssProjDialog::OnOK"));
  }
  Destroy();
}
