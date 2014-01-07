#include "BodyCache.h"
#include "BodyProvider.h"

BodyCache::BodyCache()
: m_handle(NULL)
, m_gbody(NULL)
{}

BodyCache::BodyCache(OBJHANDLE handle, GBODY * gbody)
: m_handle(handle)
, m_gbody(gbody)
{}

BodyCache::~BodyCache(){}



bool BodyCache::NeedsUpdate( OBJHANDLE handle )
{
    //return true; // For testing
    return handle != m_handle;
}

GBODY * BodyCache::Gbody()
{
    return m_gbody;
}

/*
OBJHANDLE BodyCache::Handle()
{
    return m_handle;
}
*/
