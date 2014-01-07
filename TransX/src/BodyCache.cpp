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

BodyCache::BodyCache(OBJHANDLE handle, BodyProvider * bodyProvider)
: m_handle(handle)
, m_gbody(bodyProvider->GetBody(handle))
{}

BodyCache::~BodyCache(){}



bool BodyCache::NeedsUpdate( OBJHANDLE handle )
{
    return handle != m_handle;
    //return true;
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
