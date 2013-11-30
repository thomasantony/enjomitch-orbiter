#ifndef MovementParams_H
#define MovementParams_H

#include <orbitersdk.h>
#include "Systems/Point.hpp"

struct MovementParams
{
    MovementParams( const VESSEL * vessel );

    void Update( const VESSEL * vessel );

    VECTOR3 m_pos, m_vel;
    double m_velMod;
    EnjoLib::Point current_vel;
    double current_vel_module;
    double m_lon, m_lat, m_rad;
    ELEMENTS ele;
    ORBITPARAM opar;
    VESSELSTATUS2 vesselStatus;

};

#endif // MovementParams_H
