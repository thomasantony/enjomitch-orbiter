/***************************************************************
 * Name:      da_solverApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Szymon Ender ()
 * Created:   2009-12-20
 * Copyright: Szymon Ender ()
 * License: GPLv3
 **************************************************************/
#include "wx_pch.h"

#include "da_solverApp.h"
#include "da_solverMain.h"
#ifndef WX_PRECOMP
//(*AppHeaders
#include "da_solverMain.h"
#include <wx/image.h>
//*)
#endif

IMPLEMENT_APP(da_solverApp);

bool da_solverApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
 //   wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	da_solverFrame* Frame = new da_solverFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
