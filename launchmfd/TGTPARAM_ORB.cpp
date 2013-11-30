#include "TGTPARAM_ORB.h"
#include <Orbiter/SpaceMathOrbiter.hpp>
#include <Math/SpaceMath.hpp>
#include <Math/GeneralMath.hpp>
#include <Systems/Geo.hpp>
#include "MFDDataLaunchMFD.hpp"
using namespace EnjoLib;

TGTPARAM_ORB::TGTPARAM_ORB( double inclPrev )
{
    incl = inclPrev;
}

TGTPARAM_ORB::TGTPARAM_ORB( MFDDataLaunchMFD * data, double inclPrev )
{
    TGTPARAM_ORB tgtParamLocal( inclPrev );
    if ( const OBJHANDLE hTarget = data->GetTargetHandle() )
    {
        SpaceMathOrbiter smOrb;
        //VECTOR3 targetPos, targetVel;
        const OBJHANDLE hRef = data->hRef;
        const kostStateVector & tgtSVec = smOrb.GetRelativeStateVector( hTarget, hRef );
        const SpaceMathOrbiter::ElementsOrbitParam & elemOp = smOrb.GetElements(hTarget, hRef, FRAME_EQU);

        tgtParamLocal.incl = elemOp.el.i;
        tgtParamLocal.TrL = elemOp.op.TrL;
        tgtParamLocal.rad = length(tgtSVec.pos);
        tgtParamLocal.velRad = length(tgtSVec.vel) / tgtParamLocal.rad;

        const MovementParams & mov = data->GetMovParams();
        // Get the orbital axis vector for the target
        const VECTOR3 & targetAxis = crossp(tgtSVec.pos, tgtSVec.vel);
        // Get the orbital axis for the vessel
        const VECTOR3 & vesselAxis = crossp(mov.m_pos, mov.m_vel);
        // the intersecting node is perpendicular to both axes, ie, the cross product of the two axes
        VECTOR3 node = crossp(vesselAxis, targetAxis);
        node /= length(node);

        tgtParamLocal.azFlag = CalcLaunchAzimuth( data );

        double angle;
        double period;
        if ( ! data->launched )
        {
            Geo shipPos(mov.m_lat, mov.ele.omegab + PI);
            const bool isNorthernHeading = tgtParamLocal.azFlag == NORTHERN_AZIMUTH;
            angle = SpaceMath().CalcAngleToSatelliteNodeOnGround(shipPos, tgtParamLocal.incl, elemOp.el.theta, isNorthernHeading);
            period = data->m_bodyPhys.sidDay; // we're rollin' as the ground does
        }
        else
        {
            // Get the angle between the current vessel position and the node
            angle = acos(dotp(node, mov.m_pos / length(mov.m_pos)));
            const double speed = mov.m_velMod;
            period = data->m_bodyPhys.radius / speed * 2 * PI;
            if ( angle > PI / 2 )
            {
                // We are approaching the Descending Node
                angle = PI - angle;
                node = -node;
            }
        }
        // Get the time for the vessel to get to this node
        // FIXME This is an approximation that assumes that the orbit is circular - ie constant orbital speed
        double timeToNode = angle / (2 * PI) * period;

        const VECTOR3 & futurePos = mov.m_pos + mov.m_vel;
        const double futureAngle = acos(dotp(node, futurePos / length(futurePos)));
        // If the angle-to-node of the vessel in one seconds time is greater than the angle
        // now, we have gone past the node. Flip the sign of the time error
        if( futureAngle > angle && data->launched )
            timeToNode = -timeToNode;

        tgtParamLocal.angle = angle;
        tgtParamLocal.timeToNode = timeToNode;

        VESSEL * target = data->GetTargetVessel();
        if ( target && target->NonsphericalGravityEnabled() && ! data->launched )
        {
            if ( oapiGetPlanetJCoeffCount(hRef) )
            {
                const double J2 = oapiGetPlanetJCoeff (hRef, 0);
                const double & e = elemOp.el.e;
                const double & a = elemOp.el.a;
                const double & T = elemOp.op.T;
                const double & r = data->m_bodyPhys.radius;
                const double & i = tgtParamLocal.incl;
                // from orbiter/doc/technotes/gravity.pdf :
                const double LANDriftMean = -3*2*PI/T/2.0*(r/a)*(r/a)*cos(i)/(1-e*e)/(1-e*e)*J2;
                const double LANDriftInTime = (LANDriftMean * timeToNode )/ (2 * PI) * period;
                tgtParamLocal.timeToNode += LANDriftInTime;
            }
        }
    } // if success
    *this = tgtParamLocal;
}

LAUNCH_AZIMUTH TGTPARAM_ORB::CalcLaunchAzimuth( MFDDataLaunchMFD * data )
{
    const MovementParams & mov = data->GetMovParams();
    if (data->m_isTgtLowInclination && mov.m_lat != 0)
    {
        // In this case, we can only use the second azimuth, because the first would generate instability,
        // as you'd launch into it and make your latitude much more similar to the inclination.
        LAUNCH_AZIMUTH azFlag = mov.m_lat > 0 ? SOUTHERN_AZIMUTH : NORTHERN_AZIMUTH;
        return azFlag;
    }
    else
    {
        SpaceMathOrbiter smOrb;
        const OBJHANDLE hRef = data->hRef;

        const OBJHANDLE hTarget = data->GetTargetHandle();
        const kostStateVector & tgtSVec = smOrb.GetRelativeStateVector( hTarget, hRef );
        const kostStateVector & tgtSVecEqu = smOrb.ToEquatorial( tgtSVec, hRef );
        const VECTOR3 & targetAxisEqu = crossp(tgtSVecEqu.pos, tgtSVecEqu.vel);

        const VECTOR3 & vesselPosEqu = smOrb.ToEquatorial(mov.m_pos, hRef);
        const VECTOR3 & vesselVelEqu = smOrb.ToEquatorial(mov.m_vel, hRef);
        const VECTOR3 & vesselAxisEqu = crossp(vesselPosEqu, vesselVelEqu);
        // the intersecting node is perpendicular to both axes, ie, the cross product of the two axes
        VECTOR3 nodeEqu = crossp(vesselAxisEqu, targetAxisEqu);
        // Cartesian to Polar
        smOrb.Crt2Pol(nodeEqu);

        double difference = (nodeEqu.y - mov.ele.omegab);	//data[1] = lng of intersection , omegab = lng of Pe
        if ( data->m_bodyPhys.sidDay < 0 )
            difference = -difference;
        difference = GeneralMath().GetIn0_2PIRange( difference );
        LAUNCH_AZIMUTH azFlag = difference < PI ? NORTHERN_AZIMUTH : SOUTHERN_AZIMUTH;
        return azFlag;
    }

}
