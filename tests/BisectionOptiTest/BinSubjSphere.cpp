#include "BinSubjSphere.h"

using namespace EnjoLib;

BinSubjSphere::BinSubjSphere(const Vector & shift)
: m_shift(shift)
{
}

BinSubjSphere::~BinSubjSphere()
{
}

double BinSubjSphere::UpdateGetValue( const Vector & arg )
{
    Vector x = (arg-m_shift);
    double y = 0;
    for ( Vector::iterator it = x.begin(); it != x.end(); ++it )
        y += *it * *it;
    return y;
}
