#ifndef MYCHOICEENUM_H
#define MYCHOICEENUM_H

#include <wx/choice.h>
class EnumStringMap;

class MyChoiceEnum : public wxChoice
{
    public:
        MyChoiceEnum(const EnumStringMap & enumMap, wxWindow *parent, wxWindowID id,
                     const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize,
                     int n=0, const wxString choices[]=NULL, long style=0,
                     const wxValidator &validator=wxDefaultValidator,
                     const wxString &name=wxChoiceNameStr);
        virtual ~MyChoiceEnum();


    protected:
        virtual void OnSelection( wxCommandEvent& event );


    private:
        void Init( const EnumStringMap & enumMap );

};

#endif // MYCHOICEENUM_H
