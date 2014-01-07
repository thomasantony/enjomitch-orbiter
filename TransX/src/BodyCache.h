#ifndef BODYCACHE_H
#define BODYCACHE_H

#include <OrbiterSdk.h>

struct GBODY;
class BodyCache
{
    public:
        BodyCache(OBJHANDLE handle, GBODY * gbody);
        BodyCache();
        virtual ~BodyCache();

        bool NeedsUpdate( OBJHANDLE handle );
        //OBJHANDLE Handle();
        GBODY * Gbody();
    protected:
    private:
        OBJHANDLE m_handle;
        GBODY * m_gbody;
};

#endif // BODYCACHE_H
