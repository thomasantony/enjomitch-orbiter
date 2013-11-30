#include "wx_pch.h"

#include "VertAccWindow.h"
using namespace EnjoLib;

static double gScaleX = 2;
static double gScaleY = 10;

VertAccWindow::VertAccWindow(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style , const wxString& name):
  myScrolledWindow((wxWindow*)parent, id, pos, size, style, name)
{
  //ScrollbarsOn( GetSize() );
  //onnect(wxID_ANY,wxEVT_SIZE,wxSizeEventHandler(VertAccWindow::OnSize));

  m_colAcc1 = m_colorDB.Find(_T("GREEN"));
  m_colConst = m_colorDB.Find(_T("ORANGE"));
  m_colAcc2 = m_colorDB.Find(_T("FOREST GREEN"));
}

VertAccWindow::~VertAccWindow()
{

}

static const int gPixelsFromBottom = 5;

void VertAccWindow::OnDraw (wxDC& dcUnused)
{
  wxClientDC dc(this); // helps when scrolling
  dc.SetPen(*wxBLACK_PEN);
  wxSize size = GetClientSize();
  int startX, startY, unitX, unitY;
  GetViewStart( &startX, &startY );
  GetScrollPixelsPerUnit( &unitX, &unitY );
  startX *= unitX;
  startY *= unitY; // zero if Y scrollbar is untouched


  int w = size.GetWidth();
//  int h = size.GetHeight();

  int y = size.GetHeight() - startY;

  {
    int x1 = 0 - startX;
    int x2 = w - startX;
    int yz =  y - gPixelsFromBottom;
    dc.SetPen(*wxLIGHT_GREY_PEN);
    dc.DrawLine( x1, yz, x2, yz );
    dc.SetTextForeground( *wxLIGHT_GREY );
    dc.DrawText(_T("zero vertical acceleration"), x1 + 5, yz - 20);
  }

  const int legendPosX = w - 130 - startX;
  int legendPosY = - startY;
  int legendPosYIncr = 15;
  dc.SetTextForeground( m_colAcc1 );
  dc.DrawText(_T("Initial accel"), legendPosX, legendPosY);
  legendPosY += legendPosYIncr;
  dc.SetTextForeground( m_colConst );
  dc.DrawText(_T("Const vel flight"), legendPosX, legendPosY);
  legendPosY += legendPosYIncr;
  dc.SetTextForeground( m_colAcc2 );
  dc.DrawText(_T("Acc to equalise"), legendPosX, legendPosY);

  dc.SetPen(m_colConst);
  dc.DrawLine( av_constTStartScaled - startX, y - av_constScaled, av_constTEndScaled - startX, y - av_constScaled );
//  dc.SetTextForeground( *wx );



  dc.SetPen(m_colAcc1);

  for ( size_t i = 1; i < vGraphAccX.size(); i++ )
  {
      int x1 = (int)vGraphAccX[i-1] - startX;
      int y1 = y  - (int)(vGraphAccY[i-1]);
      int x2 = (int)vGraphAccX[i] - startX;
      int y2 = y - (int)(vGraphAccY[i]);
      dc.DrawLine( x1, y1, x2, y2 );
  }

  dc.SetPen(m_colAcc2);

  for ( size_t i = 1; i < vGraphEqualizingX.size(); i++ )
  {
      int x1 = (int)vGraphEqualizingX[i-1] - startX;
      int y1 = y - (int)(vGraphEqualizingY[i-1]);
      int x2 = (int)vGraphEqualizingX[i] - startX;
      int y2 = y - (int)(vGraphEqualizingY[i]);
      dc.DrawLine( x1, y1, x2, y2 );
  }

}



static double DivideX( double in )
{
  return in / gScaleX;
}
static double DivideY( double in )
{
  return in * gScaleY + gPixelsFromBottom;// + gScaleY;
}

void VertAccWindow::SetCurrentSolutionsFunctionValues( const std::vector <Point> & parvGraphAccValues,
                                    const std::vector <Point> & parvGraphEqualizingValues )
{

  this->vGraphAccValues = parvGraphAccValues;
  this->vGraphEqualizingValues = parvGraphEqualizingValues;

  av_constTStart = parvGraphAccValues.back().x;
  av_constTEnd = parvGraphEqualizingValues.front().x;
}

void VertAccWindow::SetConstantAcceleration( double acc )
{
  av_const = acc;
}

void VertAccWindow::ScaleValues()
{
  vGraphAccX.clear(); vGraphAccY.clear(); vGraphEqualizingX.clear(); vGraphEqualizingY.clear();
  for (size_t i = 0; i < vGraphAccValues.size(); ++i)
  {
    vGraphAccX.push_back(DivideX(vGraphAccValues[i].x));
    vGraphAccY.push_back(DivideY(vGraphAccValues[i].y));
  }
  for (size_t i = 0; i < vGraphEqualizingValues.size(); ++i)
  {
    vGraphEqualizingX.push_back(DivideX(vGraphEqualizingValues[i].x));
    vGraphEqualizingY.push_back(DivideY(vGraphEqualizingValues[i].y));
  }

  av_constTStartScaled = DivideX(av_constTStart);
  av_constTEndScaled = DivideX(av_constTEnd);
  av_constScaled = DivideY(av_const);
}

// Ustawia suwaki do Scrolled Window
void VertAccWindow::ScrollbarsOn( const wxSize & size )
{
  int unit = 20;
  int pixelsAmount = 6 * size.GetWidth() / (double) unit;
  SetScrollbars(unit, unit, pixelsAmount,  pixelsAmount);

 // wxMessageBox(wxString::Format(_T("sz = %d, client sz = %d"), size.GetHeight(), GetClientSize().GetHeight()));
  double g = 10;
  gScaleY = size.GetHeight() / g;
  ScaleValues();
}

void VertAccWindow::Refresh(bool eraseBackground, const wxRect* rect)
{
  ScaleValues();

  myScrolledWindow::Refresh(eraseBackground, rect);
}

//void VertAccWindow::OnSize(wxSizeEvent& event)
//{
//  ScrollbarsOn( event.GetSize() );
//}
