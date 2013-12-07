#include "BinSearchOpti.hpp"
#include "BinSearchOptiSubject.hpp"
#include "GeneralMath.hpp"

#include <cmath>
#include <utility>
//#include <iostream>
using namespace std;
using namespace EnjoLib;

BinSearchOpti::BinSearchOpti(Vector minArg, Vector maxArg, double eps)
{
    //if (minArg > maxArg)
  //      std::swap(minArg, maxArg);

    this->m_minArg = minArg;
    this->m_maxArg = maxArg;
    this->m_eps = eps;
    // Bound binary seach should finish in log2(n) iterations. Let's allow for max 2 logs.
	GeneralMath gm;
    const double numSlices = gm.round( (maxArg-minArg).Len() / eps);
    this->m_maxIter = 2 * gm.round(gm.Log2(numSlices));
}

BinSearchOpti::~BinSearchOpti()
{
    //dtor
}

Result<Vector> BinSearchOpti::Run( BinSearchOptiSubject & subj ) const
{
//    GeneralMath gm;
    Vector mid;
    int iter = 0;
    Vector a = m_minArg;
    Vector b = m_maxArg;

    bool bmaxIter = false;
    do
    {
        for ( size_t dim = 0; dim < a.size(); ++dim )
        {
            // Cut the argument in slices until the value (value) is below threshold (binary search)
            mid = (a + b) / 2; // Midpoint
            Vector left = mid;
            Vector right = mid;
            left.at(dim) = (a.at(dim) + mid.at(dim)) / 2;
            right.at(dim) = (mid.at(dim) + b.at(dim)) / 2;
           // cout << a << " b = " << b << " mid " << mid <<  ", left = " << left << " right = " << right << endl;
            double valLeft = subj.UpdateGetValue(left);
            double valMid = subj.UpdateGetValue(mid);
            double valRight = subj.UpdateGetValue(right);

            if ( valLeft < valRight && valLeft < valMid)
            {
                b.at(dim) = mid.at(dim); // Narrow right border
            }
            else if ( valRight < valLeft && valRight < valMid  )
            {
                a.at(dim) = mid.at(dim); // Narrow left border
               // fmin = value;
            }
            else if ( valMid < valLeft && valMid < valRight )
            {
            	a.at(dim) = left.at(dim);
            	b.at(dim) = right.at(dim);
            }
            else
            {   // Everything is the same. Somethin' ain't right with yer function!
                return Result<Vector>(m_maxArg, false);
            }
        }
        bmaxIter = ++iter == m_maxIter;
    } while( (b-a).Len()/2 > m_eps && ! bmaxIter ); // Continue searching, until below threshold
    //sprintf(oapiDebugString(), "i = %d, maxi = %d, arg = %lf, value = %lf, pdiff = %lf",i, m_maxIter, arg, diff, prevDiff);
    if ( bmaxIter )
        return Result<Vector>(m_maxArg, false);
    else
        //return Result<Vector>(arg, subj.IsValid( arg, value ));
        return Result<Vector>(mid, true);
}
