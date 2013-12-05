#ifndef MFDVARCONSTRAINT_H
#define MFDVARCONSTRAINT_H

#include <utility>
class Constraint
{
    public:
    Constraint(double lower, double upper);
    double lower;
    double upper;
};

#endif // MFDVARCONSTRAINT_H
