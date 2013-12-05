#include "Constraint.h"

Constraint::Constraint(double lower, double upper)
{
    if (lower > upper)
        std::swap(upper, lower);

    this->lower = lower;
    this->upper = upper;
}
