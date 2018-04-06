#include "SurfaceElevationCache.h"
#include <Systems/Geo.hpp>
#include <Math/GeneralMath.hpp>

using namespace EnjoLib;

SurfaceElevationCache::SurfaceElevationCache()
: m_surfRefPrev(NULL)
{
    //ctor
}

SurfaceElevationCache::~SurfaceElevationCache()
{
    //dtor
}

double SurfaceElevationCache::GetSurfaceElevation(OBJHANDLE surfRef, const EnjoLib::Geo & loc)
{
    const double elev = oapiSurfaceElevation(surfRef, loc.lon, loc.lat);
    return elev;

    // Cache unused for now

    if (surfRef == NULL)
        return 0;
    if (m_surfRefPrev != NULL)
    {
        if (m_surfRefPrev != surfRef)
        {
            m_cache.clear();
        }
    }
    m_surfRefPrev = surfRef;



    const GeoKey key = std::make_pair(Round(loc.lon), Round(loc.lat));
    Cache::iterator cit = m_cache.find(key);
    if (cit != m_cache.end())
    {
        // Found value in cache
        cit->second.isUsed = true; // Don't delete it
        return cit->second.value;
    }
    else
    {
        // Must calculate the value first
        const double elev = oapiSurfaceElevation(surfRef, loc.lon, loc.lat);
        ValueState valState;
        valState.value = elev;
        m_cache[key] = valState;
        return elev;
    }
}

double SurfaceElevationCache::Round(double val) const
{
    const GeneralMath gmh;

    const double multipler = 2000000000;
    const double scaled = gmh.round(val * multipler);
    return scaled;
}
