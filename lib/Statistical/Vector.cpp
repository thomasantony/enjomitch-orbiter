/*
Modified BSD License

This file is a part of Statistical package and originates from:
http://sf.net/projects/enjomitchsorbit

Copyright (c) 2012, Szymon "Enjo" Ender
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
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <numeric>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include "Vector.hpp"
#include "Assertions.hpp"

Vector::Vector( const std::vector<double> & init )
{
    for ( Vector::const_iterator it = init.begin(); it != init.end(); ++it)
        push_back(*it);
}

Vector::Vector( int n )
{
    for (int i = 0; i < n; ++i)
        push_back(0);
}
Vector::Vector()
{
}

Vector::~Vector()
{
}

std::string Vector::Print() const /// TODO
{
    std::ostringstream ss;
    for ( Vector::const_iterator it = begin(); it != end(); ++it)
    {
        ss << *it;
        if ( it != end() - 1)
            ss << ", ";
    }

    return ss.str();
}

double Vector::Len() const
{
    return sqrt( SumSquares() );
}

Vector Vector::Norm() const
{
    double len = Len();
    if ( len == 0 )
        return Vector();
    return Vector(*this) /= Len();
}

double Vector::SumSquares() const
{
    double sumSquares = 0;
    for ( Vector::const_iterator it = begin(); it != end(); ++it )
        sumSquares += (*it) * (*it);

    return sumSquares;
}

double Vector::Mean() const
{
    if ( empty() )
        return 0;
    double mean = Sum() / (double) size();
    return mean;
}

Vector Vector::AdjustMean() const
{
    return operator - (Mean());
}

double Vector::Sum() const
{
    return std::accumulate(begin(), end(), 0.0);
}

Vector & Vector::operator += (const double f)
{
    for ( Vector::iterator it = begin(); it != end(); ++it )
        (*it) += f;

    return *this;
}

Vector & Vector::operator -= (const double f)
{
    for ( Vector::iterator it = begin(); it != end(); ++it )
        (*it) -= f;

    return *this;
}

Vector & Vector::operator /= (const double f)
{
    for ( Vector::iterator it = begin(); it != end(); ++it )
        (*it) /= f;

    return *this;
}

Vector & Vector::operator *= (const double f)
{
    for ( Vector::iterator it = begin(); it != end(); ++it )
        (*it) *= f;

    return *this;
}

Vector & Vector::operator += (const Vector & p)
{
    Assertions().SizesEqual(*this, p, "Vector::+=");
    for (size_t i = 0; i < size(); ++i)
        at(i) += p[i];

    return *this;
}

Vector & Vector::operator -= (const Vector & p)
{
    Assertions().SizesEqual(*this, p, "Vector::-=");
    for (size_t i = 0; i < size(); ++i)
        at(i) -= p[i];

    return *this;
}

Vector Vector::operator + (const Vector & p) const
{
    Assertions().SizesEqual(*this, p, "Vector::+");
    return Vector(*this) += p;
}

Vector Vector::operator - (const Vector & p) const
{
    Assertions().SizesEqual(*this, p, "Vector::-");
    return Vector(*this) -= p;
}

Vector Vector::operator - () const
{
    Vector neg(*this);
    for ( Vector::iterator it = neg.begin(); it != neg.end(); ++it )
        (*it) = -(*it);
    return neg;
}

Vector Vector::operator - (const double f) const
{
    return Vector(*this) -= f;
}

Vector Vector::operator + (const double f) const
{
    return Vector(*this) += f;
}

Vector Vector::operator * (const double f) const
{
    return Vector(*this) *= f;
}

Vector Vector::operator / (const double f) const
{
    return Vector(*this) /= f;
}

bool Vector::operator > (const Vector & p) const
{
    return Len() > p.Len();
}

bool Vector::operator < (const Vector & p) const
{
    return Len() < p.Len();
}
