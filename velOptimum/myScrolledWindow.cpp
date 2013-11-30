#include "wx_pch.h"

#include <algorithm>
#include <utility>
#include <sstream>

#include "myScrolledWindow.h"


BEGIN_EVENT_TABLE(myScrolledWindow, wxScrolledWindow)

   // EVT_MOTION(myScrolledWindow::OnMouseMove)
   // EVT_LEFT_UP(myScrolledWindow::OnLeftUp)
END_EVENT_TABLE()

// myScrolledWindow dziedziczy z wxScrolledWindow
myScrolledWindow::myScrolledWindow(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style , const wxString& name):
  wxScrolledWindow((wxWindow*)parent, id, pos, size, style, name)
{
 // m_parent = (da_solverFrame*) parent->GetParent();
  bgCol.Set(230,240,255);
  SetBackgroundColour (bgCol);

  //ScrollbarsOn(GetSize());
  Connect(wxID_ANY,wxEVT_SIZE,wxSizeEventHandler(myScrolledWindow::OnSize));


}

myScrolledWindow::~myScrolledWindow()
{
}

void myScrolledWindow::Clear()
{
  wxClientDC dc(this);
  dc.SetBackground(wxBrush(bgCol));
  dc.Clear();
}

void myScrolledWindow::Reset()
{

}

void myScrolledWindow::OnDraw (wxDC& dc)
{

}

// Ustawia suwaki do Scrolled Window
void myScrolledWindow::ScrollbarsOn( const wxSize & size )
{
  int unit = 20;
  int pixelsAmount = 6 * size.GetWidth() / unit;
  SetScrollbars(unit, unit, pixelsAmount,  pixelsAmount);
}

void myScrolledWindow::OnSize(wxSizeEvent& event)
{
  ScrollbarsOn( GetClientSize() );
  Refresh();
}


