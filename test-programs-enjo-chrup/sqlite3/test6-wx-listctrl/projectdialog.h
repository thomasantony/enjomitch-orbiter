#ifndef _PROJDIALOG_H
#define _PROJDIALOG_H

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

using namespace std;

class ProjectDialog : public wxDialog
{
public:
  ProjectDialog(wxWindow * parent, const wxString& title, wxSQLite3Database* db, long projid);
  void OnOK(wxCommandEvent & event);
  
  private:
    void InitCheckListbox();
    
    wxWindow * m_parent;
    wxCheckListBox * m_asslbox;
    wxSQLite3Database * m_db;
    long m_projid;
    wxArrayLong m_arrUids; // workers ids
    wxArrayLong m_arrAssProj; // assigned projects
    enum
    {
      ID_ASSIGNMENT_LISTBOX = 1
    };
};

#endif
