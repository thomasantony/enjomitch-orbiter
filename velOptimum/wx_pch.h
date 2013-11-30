/***************************************************************
 * Name:      wx_pch.h
 * Purpose:   Header to create Pre-Compiled Header (PCH)
 * Author:     ()
 * Created:   2010-08-18
 * Copyright:  ()
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
  #include <wx/wx.h>
  #include <wx/msgdlg.h>
  #include <wx/string.h>
  #include <wx/intl.h>
  #include <wx/sizer.h>
  #include <wx/button.h>
  #include <wx/menu.h>
  #include <wx/panel.h>
  #include <wx/scrolwin.h>
  #include <wx/statusbr.h>
  #include <wx/frame.h>
  #include <wx/stattext.h>
  #include <wx/textctrl.h>
  #include <wx/gdicmn.h>
#endif // WX_PRECOMP

#endif // WX_PCH_H_INCLUDED
