#include "MovementParams.h"
#include <Orbiter/SpaceMathOrbiter.hpp>

using namespace EnjoLib;

MovementParams::MovementParams( const VESSEL * vessel )
{
    Update(vessel);
}

// must be called before calling calculate_azimuth
void MovementParams::Update( const VESSEL * vessel )
{
    VECTOR3 pos, vel;
    OBJHANDLE hRef = vessel->GetSurfaceRef();
    vessel->GetRelativePos (hRef, pos);
    vessel->GetRelativeVel (hRef, vel);
    m_pos = pos;
    m_vel = vel;
    m_velMod = length(vel);
    pos = SpaceMathOrbiter().ToEquatorial(pos, hRef);
    vel = SpaceMathOrbiter().ToEquatorial(vel, hRef);
    SpaceMathOrbiter().Crt2Pol (pos, vel); // translate pos. and vel. from cartesian to polar
    // could use the following, but we need the velocity as well
    //vessel->GetEquPos(m_dLongitude,m_dLatitude,m_dRadius);
    m_rad = pos.x;
    m_lon = pos.y;
    m_lat = pos.z;

    current_vel.x = vel.y * m_rad * cos(m_lat); //East
    current_vel.y = vel.z * m_rad;  // North
    current_vel_module = current_vel.len();

    vessel->GetElements(NULL, ele, &opar, 0,FRAME_EQU);

    this->vesselStatus.version = 2;
    this->vesselStatus.flag = 0;
    vessel->GetStatusEx(&this->vesselStatus);
}
