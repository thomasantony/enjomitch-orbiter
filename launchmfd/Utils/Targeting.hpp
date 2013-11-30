#ifndef TARGETING_H
#define TARGETING_H

#include <string>
#include <OrbiterSDK.h>

class Targeting
{
    public:
        Targeting();
        virtual ~Targeting();

        OBJHANDLE GetTargetBody( const std::string & strTgt ) const;
        OBJHANDLE GetTargetBody( char * strTgt ) const;
        VESSEL * GetTargetVessel( const std::string & strTgt ) const;
        VESSEL * GetTargetVessel( char * strTgt ) const;
        char * ToCharArray( const std::string & str ) const;
        std::string GetName( const OBJHANDLE hObj ) const;
    protected:
    private:

};

#endif // TARGETING_H
