#ifndef TOPOMAP_H
#define TOPOMAP_H

#include <OrbiterSDK.h>

class TopoMap
{
    public:
        TopoMap();
        virtual ~TopoMap();
        void Draw(int width, int height, oapi::Sketchpad *skp) const;
    protected:
    private:
};

#endif // TOPOMAP_H
