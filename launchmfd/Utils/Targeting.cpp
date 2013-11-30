#include "Targeting.hpp"

Targeting::Targeting()
{
}

Targeting::~Targeting()
{
}

OBJHANDLE Targeting::GetTargetBody( const std::string & strTgt ) const
{
    return GetTargetBody( ToCharArray(strTgt) );
}

OBJHANDLE Targeting::GetTargetBody( char * strTgt ) const
{
    return oapiGetGbodyByName( strTgt );
}

VESSEL * Targeting::GetTargetVessel( char * strTgt ) const
{
    VESSEL * tgt = NULL;
    OBJHANDLE hVessel = oapiGetVesselByName( strTgt );
    if (hVessel)
        tgt = oapiGetVesselInterface(hVessel);

    return tgt;
}

VESSEL * Targeting::GetTargetVessel( const std::string & strTgt ) const
{
    return GetTargetVessel( ToCharArray(strTgt) );
}

char * Targeting::ToCharArray( const std::string & str ) const
{
    return (char*)str.c_str();
}

std::string Targeting::GetName( const OBJHANDLE hObj ) const
{
    const int sz = 500;
    char targetName[sz];
    oapiGetObjectName(hObj, targetName, sz);
    return targetName;
}
