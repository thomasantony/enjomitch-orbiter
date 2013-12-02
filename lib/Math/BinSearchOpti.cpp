#include "BinSearchOpti.hpp"
#include "BinSearchOptiSubject.hpp"
#include "GeneralMath.hpp"
#include <cmath>
#include <utility>

using namespace EnjoLib;

BinSearchOpti::BinSearchOpti(double minArg, double maxArg, double eps)
{
    if (minArg > maxArg)
        std::swap(minArg, maxArg);

    this->m_minArg = minArg;
    this->m_maxArg = maxArg;
    this->m_eps = eps;
    // Bound binary seach should finish in log2(n) iterations. Let's allow for max 2 logs.
	GeneralMath gm;
    const double numSlices = gm.round( (maxArg-minArg) / eps);
    this->m_maxIter = 2 * gm.round(gm.Log2(numSlices));
}

BinSearchOpti::~BinSearchOpti()
{
    //dtor
}

Result<double> BinSearchOpti::Run( BinSearchOptiSubject & subj ) const
{
//    GeneralMath gm;
    //const double refValue = subj.GetRefValue();
    const double refValue = 0;
   // double fmin = subj.UpdateGetValue(m_minArg) - refValue;
  //  double fmax = subj.UpdateGetValue(m_maxArg) - refValue;
    double mid;
    int i = 0;
    double a = m_minArg;
    double b = m_maxArg;

    bool bmaxIter = false;
    do
    {   // Cut the argument in slices until the value (value) is below threshold (binary search)
        mid = (a + b) / 2; // Midpoint
        double left = (a + mid) / 2;
        double right = (mid + b) / 2;
        double valLeft = subj.UpdateGetValue(left);
		double valMid = subj.UpdateGetValue(mid);
        double valRight = subj.UpdateGetValue(right);
		

        if ( valLeft < valRight && valLeft < valMid )
        {
            b = mid; // Narrow right border
        }
        else if ( valRight < valLeft && valRight < valMid )
        {
            a = mid; // Narrow left border
           // fmin = value;
        } else
		{
			a = left;
			b = right;
		}

        bmaxIter = ++i == m_maxIter;
    } while( (b-a)/2 > m_eps && ! bmaxIter ); // Continue searching, until below threshold
    //sprintf(oapiDebugString(), "i = %d, maxi = %d, arg = %lf, value = %lf, pdiff = %lf",i, m_maxIter, arg, diff, prevDiff);
    if ( bmaxIter )
        return Result<double>(m_maxArg, false);
    else
        //return Result<double>(arg, subj.IsValid( arg, value ));
        return Result<double>(mid, true);
}
