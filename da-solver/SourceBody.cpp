#include "SourceBody.hpp"

SourceBody::SourceBody( BODY_TYPE type )
{
    double div = 1.5;
    switch (type)
    {
        case EARTH:
            mass = 5.973698968e24;
            radius = 6.37101e6;
            sidDay = 86164.09;
        break;
        case EARTH_FASTER:
            *this = SourceBody(EARTH);
            this->sidDay /= div;
        break;
        case MARS:
            mass = 6.418542e+23;
            radius = 3.38992e6;
            sidDay = 88642.66435;
        break;
        case MARS_FASTER:
            *this = SourceBody(MARS);
            this->sidDay /= div;
        break;
        case MOON:
            mass = 7.347664e+22;
            radius = 1.738e6;
            sidDay = 2360588.15;
        break;
        case MOON_FASTER:
            *this = SourceBody(MOON);
            this->sidDay /= div;
        break;
        case END:
        break;
    }
}

SourceBody::SourceBody()
{

}

MapBody::MapBody()
{
    this->operator[]( SourceBody::EARTH ) = "Earth";
    this->operator[]( SourceBody::MOON ) = "Moon";
    this->operator[]( SourceBody::MARS ) = "Mars";
    this->operator[]( SourceBody::EARTH_FASTER ) = "Earth faster";
    this->operator[]( SourceBody::MARS_FASTER ) = "Mars faster";
    this->operator[]( SourceBody::MOON_FASTER ) = "Moon faster";
};
MapBody::~MapBody() {};
