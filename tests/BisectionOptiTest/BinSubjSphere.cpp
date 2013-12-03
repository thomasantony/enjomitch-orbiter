#include "BinSubjSphere.h"

BinSubjSphere::BinSubjSphere(double shift)
: m_shift(shift)
{
}

BinSubjSphere::~BinSubjSphere()
{
}

double BinSubjSphere::UpdateGetValue( double arg )
{
    double x = (arg-m_shift);
    double y = x*x;
    return y;
}
