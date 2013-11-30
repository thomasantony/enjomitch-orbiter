//wxHatch CVS $Id: canvas.h,v 1.2 2002/05/10 15:49:31 biol75 Exp $
//Default canvas.h for wxHatch
#ifndef _canvas_h
#define _canvas_h

#ifndef WX_PRECOMP
#include <vector>
#endif

using namespace std;

#include "../launchmfd/LegacyCode/DirectAscent/DirectAscent.h"
#include "../libwx/MyWxScrolledWindow.h"
#include "../lib/Systems/RectangleMy.hpp"

class da_solverFrame;
class wxWindow;

class myScrolledWindow: public EnjoLib::MyWxScrolledWindow, public DirectAscent
{
public:
    myScrolledWindow(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style , const wxString& name);
    virtual ~myScrolledWindow();

    //void OnLeftUp (wxMouseEvent & event);
    //void OnMouseMove (wxMouseEvent & event);
    void ScrollbarsOn( const wxSize & size );
    void DebugCalcAngDist();

    void PutPixel(int x, int y);
    void CalculateDirectAscent();
    void CalculateDirectAscentFuelEfficient();
    void DrawInnerLoop(wxDC & dc);
    void MediumLoop( bool bClean, bool fuelEfficient );
    void InnerLoop( bool fuelEfficient );
    wxString OutputValues();
    void GetUserValues();

    void Reset();
    bool Failed();
    void CenterView();
    EnjoLib::RectangleMy GetDataToDisplaySize() const;
    EnjoLib::RectangleMy GetDataSize() const;
    bool IsSymmetricAround00() const;

protected:

    void ScaleData();
    double GetShipsInitialAcceleration();


private:
    string CombineStringAndValue(const string & a, double val, const string & unit = string());

    void MyDraw( wxDC & dc );
    void DrawGC( const std::vector<EnjoLib::Point> & gcData, wxDC & dc, const wxPen * pen);
    void OnSize (wxSizeEvent& event);

    void AddPathToVectors();
    void ClearPathVectors();
    void DrawAllPaths();

    bool InitInternalVars();
    void Init();
    void ScaleGC( bool bShipOnly = false );
    void AntiScalePathGC();

    da_solverFrame * m_parent;

    wxColor bgCol;
    double timeOffset;

    vector < vector <EnjoLib::Point> > vvs;
    vector < vector <EnjoLib::Point> > vvsScaled;
    std::vector <EnjoLib::Point> vGCsatScaled; // sat's path (great circle)
    std::vector <EnjoLib::Point> vGCshScaled; // ship's path (great circle)
    std::vector <EnjoLib::Point> vGCshStartScaled; // ship's starting path (great circle)

    vector < bool > vxvel_excessed;

    bool firstRun;

    DECLARE_EVENT_TABLE()
};

#endif
