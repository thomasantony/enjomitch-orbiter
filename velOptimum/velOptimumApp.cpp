/***************************************************************
 * Name:      velOptimumApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2010-08-18
 * Copyright:  ()
 * License:
 **************************************************************/

#include "wx_pch.h"
#include "velOptimumApp.h"

//(*AppHeaders
#include "velOptimumMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(velOptimumApp);

bool velOptimumApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    //wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	velOptimumFrame* Frame = new velOptimumFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
