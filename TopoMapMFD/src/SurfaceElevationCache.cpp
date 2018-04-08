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

double SurfaceElevationCache::GetSurfaceElevation(OBJHANDLE surfRef, const Geo & loc, const Geo & topLeft, const Geo & botRight, int W, int H)
{
    //const double elev = oapiSurfaceElevation(surfRef, loc.lon, loc.lat);
    //return elev; // Uncomment the above values not to use the cache

    if (surfRef == NULL)
        return 0;
    if (m_surfRefPrev != NULL)
    {
        if (m_surfRefPrev != surfRef)
        {
            // Clear the cache, if planet changed
            m_cache.clear();
        }
    }
    m_surfRefPrev = surfRef;


    const GeoKey key = DiscretiseGetKey(loc, topLeft, botRight, W, H);
    Cache::iterator it = m_cache.find(key);
    if (it != m_cache.end())
    {
        // Found value in cache
        it->second.lastUsedTime = oapiGetSysTime(); // Don't delete it
        return it->second.value;
    }
    else
    {
        // Must calculate the value first
        const double elev = oapiSurfaceElevation(surfRef, loc.lon, loc.lat);
        m_cache[key] = ValueState(elev);
        return elev;
    }
}

SurfaceElevationCache::GeoKey SurfaceElevationCache::DiscretiseGetKey(const Geo & loc, const Geo & topLeft, const Geo & botRight, int W, int H) const
{
    // Discretisation of the geo space. This surely could be done better. Currently the poor maths affect the visuals by introducing some artifacts
    const double lenX = fabs(topLeft.lon - botRight.lon) / double(W);
    const double lenY = fabs(topLeft.lat - botRight.lat) / double(H);
	const double keyX = GeneralMath().round(loc.lon / lenX);
	const double keyY = GeneralMath().round(loc.lat / lenY);

    const GeoKey key = std::make_pair(keyX, keyY);
    return key;
}

void SurfaceElevationCache::ClearUnusedElelemts()
{
    const double MAX_AGE_SECONDS = 20;
    for(Cache::iterator it = m_cache.begin(); it != m_cache.end(); )
    {
        const double age = it->second.GetAge();
        if (age > MAX_AGE_SECONDS)
        {
            //sprintf(oapiDebugString(), "Age = %0.2lf", age);
            it = m_cache.erase(it); // Erase and return next
        }
        else
        {
            ++it;
        }
    }
}

void SurfaceElevationCache::ClearAll()
{
    m_cache.clear();
}

SurfaceElevationCache::ValueState::ValueState(double val)
 : value(val)
 , lastUsedTime(oapiGetSimTime())
{
}

double SurfaceElevationCache::ValueState::GetAge() const
{
    const double age = oapiGetSysTime() - lastUsedTime;
    return age;
}
