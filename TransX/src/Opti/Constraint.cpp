#include "Constraint.h"

Constraint::Constraint(double lower, double upper, double precision)
{
    if (lower > upper)
        std::swap(upper, lower);

    this->lower = lower;
    this->upper = upper;
    this->precision = precision;
}
