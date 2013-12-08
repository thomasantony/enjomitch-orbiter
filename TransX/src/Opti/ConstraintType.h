#ifndef CONSTRAINTTYPE_H_INCLUDED
#define CONSTRAINTTYPE_H_INCLUDED

struct ConstraintType
{
    enum e
    {
        PROGRADE_HOHMANN,
        CHANGE_PLANE,
        OUTWARD,
        ANGLE
    };
};


#endif // CONSTRAINTTYPE_H_INCLUDED
