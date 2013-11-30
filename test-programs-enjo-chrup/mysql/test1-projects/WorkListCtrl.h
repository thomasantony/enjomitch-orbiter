#ifndef WORKLISTCTRL_H_INCLUDED
#define WORKLISTCTRL_H_INCLUDED
#include "MyListCtrl.h"

class WorkListCtrl : public MyListCtrl
{
    public:
        WorkListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator, const wxString& name);

        void SelectAll();
        void ShowListFromResult(DatabaseResultSet * q);

};



#endif // WORKLISTCTRL_H_INCLUDED
