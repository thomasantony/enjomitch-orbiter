#ifndef PROJLISTCTRL_H_INCLUDED
#define PROJLISTCTRL_H_INCLUDED
#include "MyListCtrl.h"

class ProjListCtrl : public MyListCtrl
{
    public:
        ProjListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator, const wxString& name);

        void SelectAll();
        void ShowListFromResult(DatabaseResultSet * q);

};



#endif // PROJLISTCTRL_H_INCLUDED
