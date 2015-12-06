#include "TopoMap.h"
#include "ORBITERTOOLS.h"

TopoMap::TopoMap()
{
    //ctor
}

TopoMap::~TopoMap()
{
    //dtor
}

void TopoMap::Draw(int width, int height, oapi::Sketchpad *skp) const
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
        skp->MoveTo(x+width/2,0);
        for (int y=-height/2; y<height/2; y++)
        {
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
}
