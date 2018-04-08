// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015-8 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#include "TopoMap.h"
#include "ORBITERTOOLS.h"

#include <Math/GeneralMath.hpp>
#include <Math/Colors.hpp>
#include <Systems/Point.hpp>
//#include "gcAPI.h"
//#include "Sketchpad2.h"


int TopoMap::m_numLinesPerRefresh = 2;
//int TopoMap::m_numLinesPerRefresh = 8; // testing
const int TopoMap::c_maxLinesPerRefresh = 64;
const double TopoMap::c_zoomMin = 500;
const double TopoMap::c_zoomMax = 2500;
const double TopoMap::c_zoomIncr = 1.025;
//const double TopoMap::c_zoomIncr = 10; // testing
bool TopoMap::m_zoomAuto = false;
double TopoMap::m_zoom = c_zoomMin;
double TopoMap::m_zoomPrev = m_zoom;

TopoMap::TopoMap(int width, int height)
{
    //if (!gcEnabled())
    {
        //gcInitialize();
    }
    W = width; H = height;
    // W and H must be even, or the surface doesn't get redrawn.
	if (W%2 != 0) W--;
	if (H%2 != 0) H--;
    m_rgb = false;
    m_lineRefreshed = 0;

    m_elevHighest = 5000;
    m_elevLowest = -5000;
    m_surface = oapiCreateSurface(W, H);
    //m_surface = ogciCreateSurfaceEx(W, H, OAPISURFACE_TEXTURE|OAPISURFACE_RENDERTARGET); // Using Jarmo's stuff
    if (m_surface)
        oapiColourFill (m_surface, 0);
}
TopoMap::~TopoMap()
{
    if (m_surface)
        oapiDestroySurface(m_surface);
}

void TopoMap::Draw(oapi::Sketchpad *skp)
{
    if (!m_surface)
        return;
    //ogciSketchBlt(skp, m_surface, 0, 0); // Jarmo's blitting
    oapiBlt(skp->GetSurface(), m_surface, 0, 0, 0, 0, W, H); // The only working solution
    //oapiBlt(m_surface, 0, 0, 0, 0, W, H); // No effect, as the DC is locked.
    //Sketchpad2  skp2 (skp->GetSurface());
    //skp2.CopyRect(m_surface, NULL, 0, 0); // Unimplemented in DX9
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

void TopoMap::SetZoomInRange()
{
    if (m_zoom > c_zoomMax)
        m_zoom = c_zoomMax;
    else
    if (m_zoom < c_zoomMin)
        m_zoom = c_zoomMin;
}

void TopoMap::UpdateMap()
{
	if (!m_surface)
		return;
    using namespace EnjoLib;
    const VESSEL * v = oapiGetFocusInterface();
    if (! v)
        return;
    const OBJHANDLE surfRef = v->GetSurfaceRef();
    if (!surfRef)
        return;

    double lng_Vessel, lat_Vessel, rad;
    v->GetEquPos(lng_Vessel,lat_Vessel,rad);
    if (m_zoomAuto)
    {
        const double alt = rad - oapiGetSize(surfRef);
        const double zoom = alt / 11.0;
        m_zoom = zoom;
    }
    SetZoomInRange();
    //const double zoom = 500;
    //sprintf(oapiDebugString(), "Zoom = %lf", zoom);
    const double heading = ORBITERTOOLS::getFlightVectorHeading(v);

    const int xMin = (int)ceil(-W/2.0);
    const int xMax = (int)floor(W/2.0);
    const int yMin = (int)ceil(-H/2.0);
    const int yMax = (int)floor(H/2.0);

    const Geo vesGeo(lat_Vessel,lng_Vessel);
    if (m_lineRefreshed >= W) // Full cycle achieved. Start from left border again
    {
        m_lineRefreshed = 0;
        m_cache.ClearUnusedElelemts(); // Clear to save ram. Don't call this too often because it's a CPU hog
    }
    if (m_zoomPrev != m_zoom)
    {
        // When changing zoom, clear the cache to avoid artifacts
        // Unfortunately when using auto zoom, the cache is cleared automatically here, until zoom settles down
        m_cache.ClearAll();
    }

    // Variables needed for cache's geo space discretisation
    const Geo xMinGeo       = ORBITERTOOLS::pointRadialDistance(vesGeo,PI05,xMin*m_zoom,v);
    const Geo xMaxGeo       = ORBITERTOOLS::pointRadialDistance(vesGeo,PI05,xMax*m_zoom,v);
    const Geo pixelTopLeft  = ORBITERTOOLS::pointRadialDistance(xMinGeo,0,  yMin*m_zoom,v);
    const Geo pixelBotRigh  = ORBITERTOOLS::pointRadialDistance(xMaxGeo,0,  yMax*m_zoom,v);

    for (int i = 0, x=xMin + m_lineRefreshed; x<xMax; x++, m_lineRefreshed++, i++)
    {
        const Geo left = ORBITERTOOLS::pointRadialDistance(vesGeo,heading+PI05,x*m_zoom,v);
        for (int y=yMin; y<yMax; y++)
        {
            const Geo pixel = ORBITERTOOLS::pointRadialDistance(left,heading,y*m_zoom,v);
            const double elevation = m_cache.GetSurfaceElevation(surfRef, pixel, pixelTopLeft, pixelBotRigh, W, H);
            if (elevation > m_elevHighest) m_elevHighest = elevation;
            if (elevation < m_elevLowest)  m_elevLowest =  elevation;
            const double f = 255;
            const double elevation255 = ((elevation + (0 - m_elevLowest)) / ( m_elevHighest - m_elevLowest)) * f;
            DWORD col;
            if (!m_rgb)
                col = RGB(elevation255,elevation255,elevation255);
            else
            {
                const double v = (f - elevation255)/f;
                const Colors::COLOUR rgbCol = Colors().GreyToRGB(v, 0, 1);
                col = RGB(rgbCol.r * f, rgbCol.g * f, rgbCol.b * f);
            }
            int xx = x+xMax;
            int yy = H-(y+yMax) - 1;
            if (xx>W-1) xx=W-1; if (xx<0) xx=0;
            if (yy>H-1) yy=H-1; if (yy<0) yy=0;
            oapiColourFill (m_surface, col, xx, yy, 1, 1);
        }
        if (i == m_numLinesPerRefresh)
		{
			m_lineRefreshed++;
            break;
		}
    }
    m_zoomPrev = m_zoom;
}

void TopoMap::ZoomAutoSwitch()
{
    m_zoomAuto = ! m_zoomAuto;
}
void TopoMap::ZoomIn()
{
    m_zoomAuto = false;
    m_zoom /= c_zoomIncr;
}
void TopoMap::ZoomOut()
{
    m_zoomAuto = false;
    m_zoom *= c_zoomIncr;
}
void TopoMap::ZoomMaximal()
{
    m_zoomAuto = false;
    m_zoom = c_zoomMin;
}
