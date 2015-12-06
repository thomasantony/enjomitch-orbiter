#ifndef TOPOMAP_H
#define TOPOMAP_H

#include <OrbiterSDK.h>
#include <map>

class TopoMap
{
    public:
        TopoMap();
        virtual ~TopoMap();
        void Draw(int width, int height, oapi::Sketchpad *skp) const;
        void UpdateMap(int width, int height);

        struct Coords
        {
            Coords(){}
           Coords(int x, int y) : x(x), y(y) {}
           int x, y;
        };
        typedef std::multimap<int, Coords> ElevMap;
        void CalcMap(int width, int height, ElevMap * emap) const;
    protected:
    private:

        double m_prevTime;
        ElevMap m_map;
};

#endif // TOPOMAP_H
