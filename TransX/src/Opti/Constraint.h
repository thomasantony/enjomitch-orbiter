#ifndef MFDVARCONSTRAINT_H
#define MFDVARCONSTRAINT_H

#include <utility>
class Constraint
{
    public:
    Constraint(double lower, double upper, double precision);
    double lower;
    double upper;
    double precision;
};

#endif // MFDVARCONSTRAINT_H
