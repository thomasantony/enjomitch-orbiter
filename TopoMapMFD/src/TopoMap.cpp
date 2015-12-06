#include "TopoMap.h"
#include "ORBITERTOOLS.h"

TopoMap::TopoMap(){m_prevTime = 0;}
TopoMap::~TopoMap(){}

void TopoMap::UpdateMap(int width, int height)
{
    if (m_prevTime == 0)
    {
        m_prevTime = oapiGetSysTime();
        CalcMap(width, height, &m_map);
        return;
    }
    if (oapiGetSysTime() - m_prevTime < 5)
        return; // Don't calculate more often than n seconds
    m_prevTime = oapiGetSysTime();
    CalcMap(width, height, &m_map);
}

void TopoMap::CalcMap(int width, int height, TopoMap::ElevMap * yoMap) const
{
    static double highest = 5000;
    static double lowest = -5000;
    VESSEL*v = oapiGetFocusInterface();
    double lng_Vessel, lat_Vessel, rad;
    v->GetEquPos(lng_Vessel,lat_Vessel,rad);
    double heading = ORBITERTOOLS::getFlightVectorHeading(v);
    const double zoom = 500;

    for (int x=-width/2; x<width/2; x++)
    {
        double lng_left, lat_left;
        ORBITERTOOLS::pointRadialDistance(lat_Vessel,lng_Vessel,heading+PI05,x*zoom,v,&lat_left,&lng_left);
        for (int y=-height/2; y<height/2; y++)
        {
            double lng_pixel, lat_pixel;
            ORBITERTOOLS::pointRadialDistance(lat_left,lng_left,heading,y*zoom,v,&lat_pixel,&lng_pixel);
            double elevation = oapiSurfaceElevation(v->GetGravityRef(),lng_pixel,lat_pixel);
            if (elevation > highest) highest = elevation;
            if (elevation < lowest) lowest = elevation;
            double elevation255 = ((elevation + (0 - lowest)) / ( highest - lowest)) * 255;
            int elevation255Int = int(elevation255);
            yoMap->insert(std::pair<int, Coords>(elevation255Int, Coords(x,y)));
        }
    }
}

#define NEW

#ifdef NEW
void TopoMap::Draw(int width, int height, oapi::Sketchpad *skp) const
{
    int elevation255Prev = -1;
    oapi::Pen * elevationPen = NULL;
    int numSwitches = 0;
    for (ElevMap::const_iterator it = m_map.begin(); it != m_map.end(); ++it)
    {
        const int elevation255Curr = it->first;
        if (elevation255Curr != elevation255Prev || elevation255Prev == -1)
        {
            numSwitches++;
            // Time to change or initlialize Pen
            if (elevationPen)
            {
                oapiReleasePen(elevationPen);
            }
            elevationPen = oapiCreatePen(1, 1, RGB(elevation255Curr,elevation255Curr,elevation255Curr));
            skp->SetPen(elevationPen);
            elevation255Prev = elevation255Curr;
        }
        // Obtain x and y from the Coords structure
        const int x = it->second.x;
        const int y = it->second.y;
        //skp->MoveTo(x,y);
        //skp->LineTo(x,y+1); // Note here *
        skp->MoveTo(x+width/2,height-(y+height/2));
        skp->LineTo(x+width/2,height-(y+height/2)+1);

    }
    if (elevationPen) // In case the multimap had no elements, the Pen remains NULL
    {
        oapiReleasePen(elevationPen);
    }
    sprintf(oapiDebugString(), "Num switches = %d", numSwitches);
}
#else
void TopoMap::Draw(int width, int height, oapi::Sketchpad *skp) const
{
    static double highest = 5000;
    static double lowest = -5000;
    VESSEL*v = oapiGetFocusInterface();
    double lng_Vessel, lat_Vessel, rad;
    v->GetEquPos(lng_Vessel,lat_Vessel,rad);
    double heading = ORBITERTOOLS::getFlightVectorHeading(v);
    const double zoom = 500;
    int numSwitches = 0;
    for (int x=-width/2; x<width/2; x++)
    {
        double lng_left, lat_left;
        ORBITERTOOLS::pointRadialDistance(lat_Vessel,lng_Vessel,heading+PI05,x*zoom,v,&lat_left,&lng_left);
        skp->MoveTo(x+width/2,0);
        for (int y=-height/2; y<height/2; y++)
        {
            numSwitches++;
            double lng_pixel, lat_pixel;
            ORBITERTOOLS::pointRadialDistance(lat_left,lng_left,heading,y*zoom,v,&lat_pixel,&lng_pixel);
            double elevation = oapiSurfaceElevation(v->GetGravityRef(),lng_pixel,lat_pixel);
            oapi::Pen *elevationPen;
            if (elevation > highest) highest = elevation;
            if (elevation < lowest) lowest = elevation;
            double elevation255 = ((elevation + (0 - lowest)) / ( highest - lowest)) * 255;
            elevationPen = oapiCreatePen(1, 1, RGB(elevation255,elevation255,elevation255));
            skp->SetPen(elevationPen);
            skp->LineTo(x+width/2, height-(y+height/2));
            oapiReleasePen(elevationPen);
        }
    }
    sprintf(oapiDebugString(), "Num switches = %d", numSwitches);
}
#endif
