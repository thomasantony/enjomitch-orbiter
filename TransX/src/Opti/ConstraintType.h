#ifndef CONSTRAINTTYPE_H_INCLUDED
#define CONSTRAINTTYPE_H_INCLUDED

struct ConstraintType
{
    enum e
    {
        PROGRADE_HOHMANN,
        PROGRADE_MANOEUVRE,
        CHANGE_PLANE,
        OUTWARD,
        ANGLE
    };
};


#endif // CONSTRAINTTYPE_H_INCLUDED
