#include "wx_pch.h"

#include "MyChoiceEnum.h"
#include <Util/EnumStringMap.h>

MyChoiceEnum::MyChoiceEnum(const EnumStringMap & enumMap, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size,
                           int n, const wxString choices[], long style, const wxValidator &validator,
                           const wxString &name)
: wxChoice( parent, id, pos, size, n, choices, style, validator, name )
{
    Connect(wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MyChoiceEnum::OnSelection,0,this);
    Init( enumMap );
}

MyChoiceEnum::~MyChoiceEnum()
{
    //dtor
}

void MyChoiceEnum::Init( const EnumStringMap & enumMap )
{
    Clear();
    for (unsigned i = 0; i < enumMap.size(); ++i)
       Append("");
    for ( EnumStringMap::const_iterator it = enumMap.begin(); it != enumMap.end(); ++it)
        SetString(it->first, it->second);
    SetSelection(0);
}

void MyChoiceEnum::OnSelection( wxCommandEvent& event )
{
    event.Skip();
}
