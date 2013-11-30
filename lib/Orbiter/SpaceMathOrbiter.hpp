#ifndef SPACEMATHORBITER_H
#define SPACEMATHORBITER_H

#include <orbitersdk.h>
#include <kost.h>

class SpaceMathOrbiter
{
    public:
        double GetPlaneAngle( const VESSEL * v, const OBJHANDLE hTarget ) const;
        struct ElementsOrbitParam;
        ElementsOrbitParam GetElements( const OBJHANDLE hObj, const OBJHANDLE hRef, int frame ) const;
        double GetHeadBearing( const VESSEL * v ) const;
        void Crt2Pol (VECTOR3 &pos, VECTOR3 &vel) const;
        void Crt2Pol (VECTOR3 &pos) const;
        double getdeterminant(const MATRIX3 & mat) const;
        MATRIX3 getinvmatrix(const MATRIX3 & mat) const;
        VECTOR3 ToEquatorial( const VECTOR3 & in, const OBJHANDLE hRef ) const;
        VECTOR3 GetPlaneAxis( const OBJHANDLE hObj, const OBJHANDLE hRef ) const;
        /// Swaps coordinate system
        /** Orbiter uses left hand coordinates because of DirectX legacy. */
        VECTOR3 SwapCoordinateSystem( const VECTOR3 & in ) const;
        kostStateVector SwapCoordinateSystem( const kostStateVector & in ) const;
        kostStateVector ToEquatorial( const kostStateVector & in, const OBJHANDLE hRef ) const;
        kostStateVector GetRelativeStateVector( const OBJHANDLE hObj, const OBJHANDLE hRef ) const;

    protected:
    private:

};

struct SpaceMathOrbiter::ElementsOrbitParam
{
    ElementsOrbitParam( const kostElements & pel, const kostOrbitParam & pop );
    ElementsOrbitParam();
    kostElements el;
    kostOrbitParam op;
};

#endif // SPACEMATHORBITER_H
