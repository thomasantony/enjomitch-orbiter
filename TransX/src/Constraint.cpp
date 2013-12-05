#include "Constraint.h"

Constraint::Constraint(double lower, double upper)
{
    if (upper > lower)
        std::swap(upper, lower);

    this->lower = lower;
    this->upper = upper;
}
