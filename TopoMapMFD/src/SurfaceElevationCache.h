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

        double GetSurfaceElevation(OBJHANDLE surfRef, const EnjoLib::Geo & loc, const EnjoLib::Geo & topLeft, const EnjoLib::Geo & botRight, int W, int H);
        void ClearUnusedElelemts();
        void ClearAll();

    protected:

    private:


        struct ValueState
        {
            ValueState(){}
            ValueState(double value);
            double GetAge() const;
            double value;
            double lastUsedTime;
        };

        typedef std::pair<double, double> GeoKey;
        typedef std::map<GeoKey, ValueState> Cache;

        GeoKey DiscretiseGetKey(const EnjoLib::Geo & loc, const EnjoLib::Geo & topLeft, const EnjoLib::Geo & botRight, int W, int H) const;

        OBJHANDLE m_surfRefPrev;
        Cache m_cache;
};

#endif // SURFACEELEVATIONCACHE_H
