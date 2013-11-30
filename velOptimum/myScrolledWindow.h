//wxHatch CVS $Id: canvas.h,v 1.2 2002/05/10 15:49:31 biol75 Exp $
//Default canvas.h for wxHatch
#ifndef _canvas_h
#define _canvas_h
#include <vector>
using namespace std;

#include "../launchmfd/Types.hpp"

class wxWindow;

class myScrolledWindow: public wxScrolledWindow
{
public:
    myScrolledWindow(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style , const wxString& name);
    virtual ~myScrolledWindow();

    virtual void ScrollbarsOn( const wxSize & size );
		virtual void Reset();
protected:
    virtual void OnDraw (wxDC& dc);
    virtual void OnSize (wxSizeEvent& event);
    void Clear();

    wxColor bgCol;

    wxColourDatabase m_colorDB;

    DECLARE_EVENT_TABLE()
};

#endif
