#ifndef _PROJLISTCTRL_H
#define _PROJLISTCTRL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/wxsqlite3.h>

using namespace std;

class ProjListCtrl: public wxListCtrl
{
  public:
    virtual ~ProjListCtrl();
    ProjListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos,  const wxSize& size, long style, wxSQLite3Database * db);

    void OnSelected(wxListEvent & event);
    void OnActivated(wxListEvent & event);
        
    void Add();
    void Update();
    void Delete();
        
    void SelectAll();
        
    void UpdateQuery(const wxString & query);
    void SelectQuery(const wxString & query);
        
    
        
  private:
    void ShowListFromResult(wxSQLite3ResultSet & q);
    long m_projid; // project ID
    wxWindow * m_parent;
    wxSQLite3Database * m_db;
    
};

#endif// #define _MYLISTCTRL_H
