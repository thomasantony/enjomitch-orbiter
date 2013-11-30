/*
Modified BSD License

This file is a part of Math package and originates from:
http://sf.net/projects/enjomitchsorbit

Copyright (c) 2013, Szymon "Enjo" Ender
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL SZYMON ENDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Bisection.hpp"
#include "BisectionSubject.hpp"
#include "GeneralMath.hpp"
//#include <sstream>
#include <cmath>


using namespace EnjoLib;

Bisection::Bisection( double minArg, double maxArg, double eps )
{
    if (minArg > maxArg)
        std::swap(minArg, maxArg);

    this->m_minArg = minArg;
    this->m_maxArg = maxArg;
    this->m_eps = eps;
    // Bound binary seach should finish in log2(n) iterations. Let's allow for max 2 logs.
    const double numSlices = GeneralMath().round( (maxArg-minArg) / eps);
    this->m_maxIter = 2 * GeneralMath().round(log2(numSlices));
}

Bisection::~Bisection()
{}

//#include <orbitersdk.h>
Result<double> Bisection::Run( BisectionSubject & subj ) const
{
    GeneralMath gm;
    const double refValue = subj.GetRefValue();
    double fmin = subj.UpdateGetValue(m_minArg, m_minArg) - refValue;
    double fmax = subj.UpdateGetValue(m_maxArg, m_minArg) - refValue;
    if ( gm.sign(fmin) == gm.sign(fmax) )
    {
        // The value for maximal argument should have been positive and was negative
        // or it should have been negative while its positive
        //sprintf(oapiDebugString(), "prevDiff = %lf", prevDiff);
        return Result<double>(m_maxArg, false);
    }
    double arg, value;
    int i = 0;
    double a = m_minArg;
    double b = m_maxArg;
    bool bmaxIter = false;
    do
    {   // Cut the argument in slices until the value (value) is below threshold (binary search)
        arg = (a + b) / 2; // Midpoint
        value = subj.UpdateGetValue(arg, m_minArg) - refValue;
        if ( gm.sign(fmin) == gm.sign(value) )
        {
            a = arg; // Narrow left border
            fmin = value;
        }
        else
            b = arg; // Narrow right border
        bmaxIter = ++i == m_maxIter;
    } while( (b-a)/2 > m_eps && ! bmaxIter ); // Continue searching, until below threshold
    //sprintf(oapiDebugString(), "i = %d, maxi = %d, arg = %lf, value = %lf, pdiff = %lf",i, m_maxIter, arg, diff, prevDiff);
    if ( bmaxIter )
        return Result<double>(m_maxArg, false);
    else
        return Result<double>(arg, subj.IsValid( arg, value ));
}
