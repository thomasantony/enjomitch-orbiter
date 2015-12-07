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

    protected:
    private:
        TopoMap(const TopoMap &) {}
        TopoMap & operator=(const TopoMap & other) {}

        int W, H;
        int m_lineRefreshed;
        bool m_rgb;
        SURFHANDLE m_surface;
        double highest;
        double lowest;
        static int m_numLinesPerRefresh;
        static const int c_maxLinesPerRefresh;
        static const double c_zoomMax, c_zoomMin;

};

#endif // TOPOMAP_H
