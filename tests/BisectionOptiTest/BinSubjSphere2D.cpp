#include "BinSubjSphere2D.h"

BinSubjSphere2D::BinSubjSphere2D(EnjoLib::Point shift)
: m_shift(shift)
{
}

BinSubjSphere2D::~BinSubjSphere2D()
{
}

double BinSubjSphere2D::UpdateGetValue( const EnjoLib::Point & arg )
{
    EnjoLib::Point x = (arg-m_shift);
    double y = x.x*x.x + x.y*x.y;
    return y;
}
