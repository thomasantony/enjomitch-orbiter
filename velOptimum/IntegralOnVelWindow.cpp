#include "wx_pch.h"

#include <algorithm>
#include "IntegralOnVelWindow.h"

IntegralOnVelWindow::IntegralOnVelWindow(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style , const wxString& name):
  myScrolledWindow((wxWindow*)parent, id, pos, size, style, name)
{

 // ScrollbarsOn(GetSize());

 m_orbVel = m_orbVelScaled = 0;

  m_colSum = m_colorDB.Find(_T("VIOLET RED"));
  m_colHori = m_colorDB.Find(_T("BLUE"));
  m_colVert = m_colorDB.Find(_T("RED"));
}

IntegralOnVelWindow::~IntegralOnVelWindow()
{

}



static const int gPixelsFromBottom = 2;

void IntegralOnVelWindow::OnDraw (wxDC& dcUnused)
{
  wxClientDC dc(this); // helps when scrolling
  wxSize size = GetClientSize();
  int startX, startY, unitX, unitY;
  GetViewStart( &startX, &startY );
  GetScrollPixelsPerUnit( &unitX, &unitY );
  startX *= unitX;
  startY *= unitY; // zero if Y scrollbar is untouched


  int h = size.GetHeight();
  int w = size.GetWidth();

  if ( m_orbVelScaled != 0 )
  {
    int x = m_orbVelScaled - startX;
    int y1 = h - 0 - startY;
    int y2 = 0 - startY;
    dc.SetPen(*wxLIGHT_GREY_PEN);
    dc.DrawLine( x, y1, x, y2 );
    dc.SetTextForeground( *wxLIGHT_GREY );
    dc.DrawText( _T("orbital velocity"), x + 5, y2 + 5);
  }

  {
    int x1 = 0 - startX;
    int x2 = w - startX;
    int y = h - startY - gPixelsFromBottom;
    dc.SetPen(*wxLIGHT_GREY_PEN);
    dc.DrawLine( x1, y, x2, y );
    dc.SetTextForeground( *wxLIGHT_GREY );
    dc.DrawText(_T("dv zero"), x2 - 120, y - 20);
  }

  const int legendPosX = w - 120 - startX;
  int legendPosY = - startY;
  int legendPosYIncr = 15;
  dc.SetTextForeground( m_colSum );
  dc.DrawText(_T("dv sum"), legendPosX, legendPosY);
  legendPosY += legendPosYIncr;
  dc.SetTextForeground( m_colVert );
  dc.DrawText(_T("dv vertical"), legendPosX, legendPosY);
  legendPosY += legendPosYIncr;
  dc.SetTextForeground( m_colHori );
  dc.DrawText(_T("dv horizontal"), legendPosX, legendPosY);
  legendPosY += legendPosYIncr;
  dc.SetTextForeground( *wxBLACK );
  dc.DrawText(_T("vel current"), legendPosX, legendPosY);



  dc.SetPen(m_colHori);
  for ( size_t i = 1; i < vScaledX.size() && i < vHoriScaledY.size(); i++ )
  {

      int x1 = (int)vScaledX[i-1] - startX;
      int y1 = h - (int)(vHoriScaledY[i-1]) - startY;
      int x2 = (int)vScaledX[i] - startX;
      int y2 = h - (int)(vHoriScaledY[i]) - startY;
      dc.DrawLine( x1, y1, x2, y2 );
  }

  dc.SetPen(m_colVert);
  for ( size_t i = 1; i < vScaledX.size() && i < vVertScaledY.size(); i++ )
  {

      int x1 = (int)vScaledX[i-1] - startX;
      int y1 = h - (int)(vVertScaledY[i-1]) - startY;
      int x2 = (int)vScaledX[i] - startX;
      int y2 = h - (int)(vVertScaledY[i]) - startY;
      dc.DrawLine( x1, y1, x2, y2 );
  }

  dc.SetPen(m_colSum);
  for ( size_t i = 1; i < vScaledX.size() && i < vVertScaledY.size() && i < vHoriScaledY.size(); i++ )
  {

      int x1 = (int)vScaledX[i-1] - startX;
      int y1 = h - (int)(vVertScaledY[i-1] + vHoriScaledY[i-1]) - startY;
      int x2 = (int)vScaledX[i] - startX;
      int y2 = h - (int)(vVertScaledY[i] + vHoriScaledY[i]) - startY;
      dc.DrawLine( x1, y1, x2, y2 );
  }

    dc.SetPen(*wxBLACK_PEN);

    int x = (int)pSolutionScaled.vel - startX;
    int y = h - (int)pSolutionScaled.dv() - startY;
    dc.DrawLine( x, y, x, h );
}

static double gScaleX = 100;
static double gScaleY = 100;

static double DivideX( double in )
{
  return in * gScaleX;
}

static double DivideY( double in )
{
  return in * gScaleY;
}


void IntegralOnVelWindow::Add( const VelIntegral & velIntegral )
{
  vx.push_back( velIntegral.vel );
  vHoriY.push_back( velIntegral.dvHori );
  vVertY.push_back( velIntegral.dvVert );
}

void IntegralOnVelWindow::SetCurrentSolution( const VelIntegral & velIntegral )
{
  pSolution = velIntegral;
}

void IntegralOnVelWindow::SetOrbitalVelocity( double orbVel )
{
  m_orbVel = orbVel;
  m_orbVelScaled = DivideX(m_orbVel);
}

void IntegralOnVelWindow::Reset()
{
  vx.clear();
  vVertY.clear();
  vHoriY.clear();
  Clear();

  pSolution = VelIntegral();
}

void IntegralOnVelWindow::ScaleValues()
{
  vScaledX.assign(vx.size(), 0);
  vVertScaledY.assign(vVertY.size(), 0);
  vHoriScaledY.assign(vHoriY.size(), 0);
  std::transform(vx.begin(), vx.end(), vScaledX.begin(), DivideX);
  std::transform(vVertY.begin(), vVertY.end(), vVertScaledY.begin(), DivideY);
  std::transform(vHoriY.begin(), vHoriY.end(), vHoriScaledY.begin(), DivideY);

  pSolutionScaled.vel = DivideX(pSolution.vel);
  pSolutionScaled.dvVert = DivideY(pSolution.dvVert);
  pSolutionScaled.dvHori = DivideY(pSolution.dvHori);

  m_orbVelScaled = DivideX(m_orbVel);
}

void IntegralOnVelWindow::SetScale()
{
  wxSize size = GetClientSize();
  if ( m_orbVel == 0)
  {
    gScaleX = 0.01;
    gScaleY = 0.01;
  }
  else
  {
    gScaleX = size.GetWidth() / m_orbVel / 3;
    gScaleY = size.GetHeight() / 40000.0;
    //wxMessageBox(wxString::Format(_T("sz = %.2f"), gScaleY));
  }

}

// Ustawia suwaki do Scrolled Window
void IntegralOnVelWindow::ScrollbarsOn( const wxSize & size )
{
  int unit = 20;
  int pixelsAmount = 6 * size.GetWidth() / (double) unit;
  SetScrollbars(unit, unit, pixelsAmount,  pixelsAmount);



 // wxMessageBox(wxString::Format(_T("sz = %d, client sz = %d"), size.GetHeight(), GetClientSize().GetHeight()));
 // double g = 9.3;
  SetScale();
  ScaleValues();



}

void IntegralOnVelWindow::Refresh(bool eraseBackground, const wxRect* rect)
{
  SetScale();
  ScaleValues();

  myScrolledWindow::Refresh(eraseBackground, rect);
}

//void IntegralOnVelWindow::OnSize(wxSizeEvent& event)
//{
//  ScrollbarsOn( event.GetSize() );
//}
