/***************************************************************
 * Name:      wx_pch.h
 * Purpose:   Header to create Pre-Compiled Header (PCH)
 * Author:    Szymon Ender ()
 * Created:   2010-08-22
 * Copyright: Szymon Ender ()
 * License:
 **************************************************************/

#ifndef WX_PCH_H_INCLUDED
#define WX_PCH_H_INCLUDED

#if ( !defined(WX_PRECOMP) )
    #define WX_PRECOMP
#endif

// basic wxWidgets headers
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#else
    #include <wx/app.h>
    // put here all your rarely-changing header files
    #include "../libwx/MyWxScrolledWindow.h"

    #include <algorithm>
    #include <utility>
    #include <vector>

    #include <wx/scrolwin.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
    #include <wx/radiobox.h>
    #include <wx/menu.h>
    #include <wx/textctrl.h>
    #include <wx/slider.h>
    #include <wx/panel.h>
    #include <wx/choice.h>
    #include <wx/button.h>
    #include <wx/frame.h>
    #include <wx/statusbr.h>
#endif // WX_PRECOMP

#endif // WX_PCH_H_INCLUDED
