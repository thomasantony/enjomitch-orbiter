#ifndef _MYLISTCTRL_H
#define _MYLISTCTRL_H

#include <wx/wx.h>
//#include <wx/string.h>
#include <wx/listctrl.h>
#include <wx/wxsqlite3.h>

using namespace std;

class MyListCtrl: public wxListCtrl
{
  public:
    virtual ~MyListCtrl();
    MyListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos,  const wxSize& size, long style, wxSQLite3Database * db);

        void OnSelected(wxListEvent & event);
        void OnActivated(wxListEvent & event);
        
        void Add();
        void Update();
        void Delete();
        
        void Functions(wxCommandEvent & event);
        void MoreLess(wxCommandEvent & event);
        void Between(wxCommandEvent & event);
        
        void SearchFromFields(const bool And);
        void SelectAll();
        
        void UpdateQuery(const wxString & query);
        void SelectQuery(const wxString & query);
        
        
        wxSQLite3Database * m_db;
  private:
    void ShowListFromResult(wxSQLite3ResultSet & q);
    long m_userid; // worker ID
    wxWindow * m_parent;
    
    
};

#endif// #define _MYLISTCTRL_H
