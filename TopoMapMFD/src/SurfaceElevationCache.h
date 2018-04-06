#ifndef SURFACEELEVATIONCACHE_H
#define SURFACEELEVATIONCACHE_H

#include <OrbiterAPI.h>
#include <map>

namespace EnjoLib
{
    struct Geo;
}

class SurfaceElevationCache
{
    public:
        SurfaceElevationCache();
        virtual ~SurfaceElevationCache();

        double GetSurfaceElevation(OBJHANDLE surfRef, const EnjoLib::Geo & loc);

    protected:

    private:
        double Round(double val) const;
        void ClearUnusedElelemts();

        struct ValueState
        {
            ValueState()
            : value(0)
            , isUsed(true)
            {}
            double value;
            bool isUsed;
        };
        OBJHANDLE m_surfRefPrev;
        typedef std::pair<double, double> GeoKey;
        typedef std::map<GeoKey, ValueState> Cache;
        Cache m_cache;


};

#endif // SURFACEELEVATIONCACHE_H
