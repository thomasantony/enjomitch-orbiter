// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#include "TopoMap.h"
#include "ORBITERTOOLS.h"
#include <Math/GeneralMath.hpp>
#include <Math/Colors.hpp>
#include <Systems/Point.hpp>

int TopoMap::m_numLinesPerRefresh = 2;
const int TopoMap::c_maxLinesPerRefresh = 64;
const double TopoMap::c_zoomMin = 200;
const double TopoMap::c_zoomMax = 5000;

TopoMap::TopoMap(int width, int height)
{
    W = width;
    H = height;
    m_rgb = false;
    m_lineRefreshed = 0;
    m_surface = oapiCreateSurface(W, H);
    oapiColourFill (m_surface, 0);
    highest = 5000;
    lowest = -5000;
}
TopoMap::~TopoMap()
{
    oapiDestroySurface(m_surface);
}

void TopoMap::Draw(oapi::Sketchpad *skp)
{
    oapiBlt(skp->GetSurface(), m_surface, 0, 0, 0, 0, W, H);
}

void TopoMap::RefreshIncrement()
{
    m_numLinesPerRefresh *= 2;
    if (m_numLinesPerRefresh > c_maxLinesPerRefresh)
        m_numLinesPerRefresh = c_maxLinesPerRefresh;
}

void TopoMap::RefreshDecrement()
{
    m_numLinesPerRefresh /= 2;
    if (m_numLinesPerRefresh < 1)
        m_numLinesPerRefresh = 1;
}

void TopoMap::UpdateMap()
{
    using namespace EnjoLib;
    const VESSEL * v = oapiGetFocusInterface();
    const OBJHANDLE surfRef = v->GetSurfaceRef();
    if (!surfRef)
        return;

    double lng_Vessel, lat_Vessel, rad;
    v->GetEquPos(lng_Vessel,lat_Vessel,rad);
    const double alt = rad - oapiGetSize(v->GetSurfaceRef());
    double zoom = alt / 11.0;
    if (zoom > c_zoomMax)
        zoom = c_zoomMax;
    else
    if (zoom < c_zoomMin)
        zoom = c_zoomMin;
    //const double zoom = 500;
    //sprintf(oapiDebugString(), "Zoom = %lf", zoom);
    const double heading = ORBITERTOOLS::getFlightVectorHeading(v);

    const int xMin = (int)ceil(-W/2.0);
    const int xMax = (int)floor(W/2.0);
    const int yMin = (int)ceil(-H/2.0);
    const int yMax = (int)floor(H/2.0);

    const Geo vesGeo(lat_Vessel,lng_Vessel);
    if (m_lineRefreshed >= W)
    {
        m_lineRefreshed = 0;
    }
    for (int i = 0, x=xMin + m_lineRefreshed; x<xMax; x++, m_lineRefreshed++, i++)
    {
        const Geo left = ORBITERTOOLS::pointRadialDistance(vesGeo,heading+PI05,x*zoom,v);
        for (int y=yMin; y<yMax; y++)
        {
            const Geo pixel = ORBITERTOOLS::pointRadialDistance(left,heading,y*zoom,v);
            const double elevation = oapiSurfaceElevation(v->GetSurfaceRef(),pixel.lon,pixel.lat);
            if (elevation > highest) highest = elevation;
            if (elevation < lowest)  lowest =  elevation;
            const double f = 255;
            const double elevation255 = ((elevation + (0 - lowest)) / ( highest - lowest)) * f;
            DWORD col;
            if (!m_rgb)
                col = RGB(elevation255,elevation255,elevation255);
            else
            {
                const double v = (f - elevation255)/f;
                const Colors::COLOUR rgbCol = Colors().GreyToRGB(v, 0, 1);
                col = RGB(rgbCol.r * f, rgbCol.g * f, rgbCol.b * f);
            }
            const int xx = x+xMax;
            const int yy = H-(y+yMax);
            oapiColourFill (m_surface, col, xx, yy, 1, 1);
        }
        if (i == m_numLinesPerRefresh)
		{
			m_lineRefreshed++;
            break;
		}
    }
}
