/***************************************************************
 * Name:      proj_testApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Simon Ender ()
 * Created:   2008-11-06
 * Copyright: Simon Ender ()
 * License:
 **************************************************************/

#include "proj_testApp.h"

//(*AppHeaders
#include "proj_testMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(proj_testApp);

bool proj_testApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	proj_testFrame* Frame = new proj_testFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
