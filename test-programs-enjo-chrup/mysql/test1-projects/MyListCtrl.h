#ifndef MYLISTCTRL_H_INCLUDED
#define MYLISTCTRL_H_INCLUDED
#include <wx/listctrl.h>
#include <wx/wx.h>
#include <wx/databaselayer/MysqlDatabaseLayer.h>

class MyListCtrl : public wxListCtrl
{
  public:
    MyListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style,
        const wxValidator& validator, const wxString& name);
    virtual ~MyListCtrl();

    void Query(const wxString &strQuery, bool bParseQuery = false);
    void Select(const wxString &strQuery);


  protected:
    virtual void ShowListFromResult(DatabaseResultSet * q) = 0;
    MysqlDatabaseLayer * db;
};

#endif // MYLISTCTRL_H_INCLUDED
