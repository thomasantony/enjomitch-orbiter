#include <wx/wx.h>
#include <wx/string.h>
#include <wx/databaselayer/DatabaseLayerException.h>
#include "MyListCtrl.h"
#include "proj_testMain.h"


MyListCtrl::MyListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style,
        const wxValidator& validator, const wxString& name)
        :  wxListCtrl(parent, id, pos, size, style, validator, name)
{
    // parent = Panel1, parent of Panel1 = Notebook, parent of Notebook = myFrame
    proj_testFrame * frame = (proj_testFrame *) parent->GetParent()->GetParent();
    db = &frame->db;
}

MyListCtrl::~MyListCtrl()
{

}

void MyListCtrl::Query(const wxString &strQuery, bool bParseQuery)
{
    try {
        db->RunQuery(strQuery, bParseQuery);
    }
    catch (DatabaseLayerException & e)
    {
        wxMessageBox(e.GetErrorMessage(), _("Exc MyListCtrl::Query"));
    }
}

void MyListCtrl::Select(const wxString & strQuery)
{
    try {
        //if (!db->IsOpen())  db->Open(DBFILE);
        DatabaseResultSet * q = db->RunQueryWithResults(strQuery);
        ShowListFromResult(q);
    }
    catch (DatabaseLayerException & e)
    {
        wxMessageBox(e.GetErrorMessage(), _("Exc MyListCtrl::Select"));
    }
}
