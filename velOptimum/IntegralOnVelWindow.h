#ifndef INTEGRALONVELWINDOW_H_INCLUDED
#define INTEGRALONVELWINDOW_H_INCLUDED

#include "myScrolledWindow.h"
#include "../launchmfd/LegacyCode/DirectAscent/VelIntegral.h"

class VelIntegral;

class IntegralOnVelWindow : public myScrolledWindow
{
  public:
    IntegralOnVelWindow(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style , const wxString& name);
    virtual ~IntegralOnVelWindow();

    void Add (const VelIntegral & VelIntegral);
		void SetCurrentSolution (const VelIntegral & VelIntegral);
		void SetOrbitalVelocity( double orbVel );
		virtual void Reset();
		void Refresh(bool eraseBackground = true, const wxRect* rect = NULL);

  protected:
    virtual void OnDraw (wxDC& dc);
    void ScrollbarsOn( const wxSize & size );
    void ScaleValues();
    void SetScale();
    //virtual void OnSize(wxSizeEvent& event);

    std::vector <double> vx;
    std::vector <double> vVertY;
    std::vector <double> vHoriY;
    std::vector <double> vScaledX;
    std::vector <double> vVertScaledY;
    std::vector <double> vHoriScaledY;
    double m_orbVel, m_orbVelScaled;

    VelIntegral pSolution, pSolutionScaled;

    wxColor m_colSum;
    wxColor m_colHori;
    wxColor m_colVert;

   // DECLARE_EVENT_TABLE()
};

#endif // INTEGRALONVELWINDOW_H_INCLUDED
