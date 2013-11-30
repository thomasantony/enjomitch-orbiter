#ifndef VERTACCWINDOW_H_INCLUDED
#define VERTACCWINDOW_H_INCLUDED

#include "myScrolledWindow.h"
#include "../lib/Systems/Point.hpp"
#include "../launchmfd/LegacyCode/DirectAscent/VelIntegral.h"

class VertAccWindow : public myScrolledWindow
{
public:
    VertAccWindow(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style , const wxString& name);
    virtual ~VertAccWindow();

    virtual void ScrollbarsOn( const wxSize & size );
    void Add (const VelIntegral & VelIntegral);
    void SetCurrentSolution (const VelIntegral & VelIntegral);
    void SetCurrentSolutionsFunctionValues( const std::vector <EnjoLib::Point> & vGraphAccValues,
                                            const std::vector <EnjoLib::Point> & vGraphEqualizingValues );
    void SetConstantAcceleration( double acc);
    virtual void Refresh(bool eraseBackground = true, const wxRect* rect = NULL);



protected:
    virtual void OnDraw (wxDC& dc);
    //  virtual void OnSize (wxSizeEvent& event);

    std::vector <EnjoLib::Point> vGraphAccValues;
    std::vector <EnjoLib::Point> vGraphEqualizingValues;

    std::vector <double> vGraphAccX;
    std::vector <double> vGraphAccY;
    std::vector <double> vGraphEqualizingX;
    std::vector <double> vGraphEqualizingY;

    double av_constTStart, av_constTEnd, av_const; // constanc acceleration
    double av_constTStartScaled, av_constTEndScaled, av_constScaled; // constanc acceleration

private:
    void ScaleValues();

    wxColor m_colAcc1;
    wxColor m_colConst;
    wxColor m_colAcc2;


    // DECLARE_EVENT_TABLE()
};

#endif // VERTACCWINDOW_H_INCLUDED
