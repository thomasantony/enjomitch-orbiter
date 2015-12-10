// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

#ifndef TOPOMAP_H
#define TOPOMAP_H

#include <OrbiterSDK.h>
#include <map>

class TopoMap
{
    public:
        TopoMap(int width, int height);
        virtual ~TopoMap();
        void Draw(oapi::Sketchpad *skp);
        void UpdateMap();
        void SetRGB(bool rgb) {m_rgb = rgb;};
        void RefreshIncrement();
        void RefreshDecrement();
        int GetRefreshLines() const { return m_numLinesPerRefresh; };
        double GetZoom() const { return m_zoom; }
        bool IsZoomAuto() const { return m_zoomAuto; }
        void ZoomAutoSwitch();
        void ZoomIn();
        void ZoomOut();
        void ZoomMaximal();


    protected:
    private:
        TopoMap(const TopoMap &) {}
        TopoMap & operator=(const TopoMap & other) {}

        void SetZoomInRange();

        int W, H;
        int m_lineRefreshed;
        bool m_rgb;
        SURFHANDLE m_surface;
        double highest;
        double lowest;
        static int m_numLinesPerRefresh;
        static const int c_maxLinesPerRefresh;
        static const double c_zoomMax, c_zoomMin, c_zoomIncr;
        static bool m_zoomAuto;
        static double m_zoom;

};

#endif // TOPOMAP_H
