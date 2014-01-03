#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <OrbiterSdk.h>

class MathUtil
{
    public:
        MathUtil();
        virtual ~MathUtil();
        static VECTOR3 GetRotationToTarget(VESSEL * vessel, const VECTOR3 & target);
    protected:
    private:
};

#endif // MATHUTIL_H
