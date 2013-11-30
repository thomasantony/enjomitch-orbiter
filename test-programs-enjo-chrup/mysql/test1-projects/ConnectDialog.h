#ifndef CONNECTDIALOG_H_INCLUDED
#define CONNECTDIALOG_H_INCLUDED

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/databaselayer/MysqlDatabaseLayer.h>

class ConnectDialog : public wxDialog
{
  public:
    ConnectDialog( wxWindow * parent, const wxString & title, MysqlDatabaseLayer* db );


  private:
    void OnOK( wxCommandEvent & event );

    MysqlDatabaseLayer * m_db;
    wxWindow * m_parent;
    wxTextCtrl * m_tHost;
    wxTextCtrl * m_tLogin;
    wxTextCtrl * m_tPass;
    wxTextCtrl * m_tDb;

    static const long idtHost;
    static const long idtDb;
    static const long idtLogin;
    static const long idtPass;
};


#endif // CONNECTDIALOG_H_INCLUDED
