#ifndef SOURCEBODY_HPP_INCLUDED
#define SOURCEBODY_HPP_INCLUDED

#include "../launchmfd/BODYPHYS.h"
#include <map>
#include <string>

class SourceBody : public BODYPHYS
{
public:
    enum BODY_TYPE
    {
        EARTH,
        EARTH_FASTER, // earth rotating faster
        MARS,
        MARS_FASTER,
        MOON,
        MOON_FASTER,
        END
    };

    SourceBody( BODY_TYPE type );

    private:
        SourceBody();
};

struct MapBody : std::map<unsigned int, std::string>
{
    MapBody();
    ~MapBody();
};

#endif // SOURCEBODY_HPP_INCLUDED
