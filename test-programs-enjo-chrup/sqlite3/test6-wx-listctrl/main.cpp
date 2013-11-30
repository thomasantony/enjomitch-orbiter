#include <wx/wx.h>
#include "test.h"

class MyApp : public wxApp
{
  public:
    virtual bool OnInit();
};


IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{

    Ident *ident = new Ident(wxT("Teh wonder app"));
    ident->Show(true);

    return true;
}
