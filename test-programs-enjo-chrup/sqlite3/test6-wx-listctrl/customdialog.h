#ifndef _CUSTOMDIALOG_H
#define _CUSTOMDIALOG_H

#include <wx/wx.h>
#include <wx/wxsqlite3.h>

using namespace std;

class CustomDialog : public wxDialog
{
public:
  CustomDialog(wxWindow * parent, const wxString& title, wxSQLite3Database* db, long userid);
  void OnOK(wxCommandEvent & event);
  
  private:
    void InitCheckListbox();
    
    wxWindow * m_parent;
    wxCheckListBox * m_asslbox;
    wxSQLite3Database * m_db;
    long m_userid;
    wxArrayLong m_arrPids; // project ids
    wxArrayLong m_arrAssProj; // assigned projects
    enum
    {
      ID_ASSIGNMENT_LISTBOX = 1
    };
};

#endif
