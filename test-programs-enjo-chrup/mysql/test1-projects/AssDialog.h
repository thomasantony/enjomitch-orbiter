#ifndef ASSDIALOG_H_INCLUDED
#define ASSDIALOG_H_INCLUDED

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/databaselayer/MysqlDatabaseLayer.h>

class AssDialog : public wxDialog
{
  public:
    AssDialog(wxWindow * parent, const wxString& title, MysqlDatabaseLayer* db, long id);

  protected:
    virtual void InitCheckListbox() = 0;
    void OnOK (wxCommandEvent & WXUNUSED(event));

    wxString m_str1stId, m_str2ndId;
    wxWindow * m_parent;
    wxCheckListBox * m_asslbox;
    MysqlDatabaseLayer * m_db;
    long m_id;
    wxArrayLong m_arrIds; // ids of project or workers
    wxArrayLong m_arrAssProj; // assigned projects
    static const long ID_ASSIGNMENT_LISTBOX;
};

#endif // ASSDIALOG_H_INCLUDED
