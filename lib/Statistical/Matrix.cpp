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

#include "Matrix.hpp"
#include "Assertions.hpp"

#include <sstream>
#include <iostream>

using namespace EnjoLib;

Matrix::Matrix()
{
}

Matrix::Matrix(int n)
{
    this->reserve(n);
    for (int i = 0; i < n; ++i)
        push_back( VectorD(n) );
}

Matrix::Matrix(int n, int m)
{
    this->reserve(n);
    for (int i = 0; i < n; ++i)
        push_back( VectorD(m) );
}

Matrix::~Matrix()
{
}

void Matrix::Add(const VectorD & vec)
{
    push_back( vec );
}

int Matrix::GetNRows() const
{
    return size();
}

/*
int Matrix::GetNCols() const
{
    if ( size() == 0 )
        return 0;
    return this->operator[](0).size();
}
*/

std::string Matrix::Print() const // TODO
{
    std::ostringstream ss;
    //ss << "\n"; // breaks file exchange
    for ( Matrix::const_iterator it = begin(); it != end(); ++it)
        ss << it->Print() << "\n";
    return ss.str();
}

std::string Matrix::PrintScilab( const char * varName ) const // TODO
{
    std::ostringstream ss;
    ss << "\n" << varName << " = [ ";
    for ( Matrix::const_iterator it = begin(); it != end(); ++it)
        ss << it->Print() << "\n";
    ss << " ];\n";
    return ss.str();
}

std::string Matrix::SizeStr() const
{
    std::ostringstream oss;
    oss << "(" << GetNRows() << ", " << GetNCols() << ")";
    return oss.str();
}

Matrix Matrix::T() const
{
    const int nrows = GetNRows();
    const int ncols = GetNCols();
    Matrix t(ncols, nrows);
    for ( int i = 0; i < nrows; ++i )
        for ( int j = 0; j < ncols; ++j )
            t[j][i] = at(i)[j];
    return t;
}

Matrix Matrix::FilterByMask(const std::vector<bool> & mask) const
{
    Assertions::SizesEqual(at(0), mask.size(), "Matrix::FilterByMask");

    Matrix ret;
    ret.reserve(size());
    const size_t maskSize = mask.size();
    for (size_t i = 0; i < this->size(); ++i)
    {
        VectorD row;
        row.reserve(maskSize);
        for (size_t j = 0; j < maskSize; ++j)
        {
            if (mask.at(j))
            {
                row.push_back(at(i).at(j));
                //row.push_back(this->operator[](i)[j]);
            }
        }
        ret.Add(row);
    }
    return ret;
}

Matrix & Matrix::FilterByMaskMe(const std::vector<bool> & mask)
{
    *this = FilterByMask(mask);
    return *this;
}

VectorD Matrix::Flatten() const
{
    const int nrows = GetNRows();
    const int ncols = GetNCols();
    VectorD flat;
    flat.reserve(nrows * ncols);
    for ( int i = 0; i < nrows; ++i )
        for ( int j = 0; j < ncols; ++j )
            flat.push_back(this->operator[](i)[j]);
    return flat;
}

Matrix & Matrix::TMe()
{
    *this = T();
    return *this;
}

Matrix Matrix::operator * (const Matrix & par) const
{
    Assertions::CanMultiply( *this, par, "Matrix::operator *" );
    Matrix prod(GetNRows(), par.GetNCols());
    const int nrows = prod.GetNRows();
    const int ncols = prod.GetNCols();
    const int ncolsThis = GetNCols();
    for (int i = 0; i < nrows; ++i)
        for (int j = 0; j < ncols; ++j)
        {
            double prodSum = 0;
            for (int ai = 0; ai < ncolsThis; ++ai )
            {
                //prodSum += at(i).at(ai) * par.at(ai).at(j);
                prodSum += this->operator[](i)[ai] * par[ai][j];
            }
            prod[i][j] = prodSum;
        }
    return prod;
}

Matrix Matrix::AdjustMeanRows() const
{
    const Matrix & ret = AdjustMean(*this);
    return ret;
}

Matrix Matrix::AdjustMeanCols() const
{
    const Matrix & ret = AdjustMean(T());
    return ret.T();
}

Matrix Matrix::ApplyMeanRows(const VectorD & mean) const
{
    return ApplyMean(*this, mean);
}

Matrix Matrix::ApplyMeanCols(const VectorD & mean) const
{
    const Matrix & mulT = T();
    return ApplyMean(mulT, mean);
}

Matrix Matrix::ApplyMean(const Matrix & mat2Apply, const VectorD & mean) const
{
    Matrix ret;
    Assertions::SizesEqual(mean, mat2Apply.size(), "Matrix::ApplyMean");
    for (size_t row = 0; row < mat2Apply.size(); ++row)
    {
        const VectorD & apl = mat2Apply.at(row) + mean.at(row);
        ret.push_back(apl);
    }
    return ret;
}

Matrix Matrix::AdjustMean(const Matrix & mat2Apply) const
{
    if (mat2Apply.GetNCols() == 1)
    {
        std::cout << "Warning at Matrix::AdjustMean(): Matrix of one column returns 0\n";
    }
    Matrix ret;
    for ( int i = 0; i < mat2Apply.GetNRows(); ++i )
    {
        const VectorD & apl = mat2Apply.at(i);

        Assertions::SizesEqual(apl, size(), "Matrix::AdjustMean");
        ret.push_back( apl.AdjustMean() );
    }
    return ret;
}

Matrix Matrix::AdjustMeanCols(const VectorD & mean) const
{
    Matrix ret;
    for ( int i = 0; i < GetNRows(); ++i )
    {
        const VectorD & apl = at(i);

        Assertions::SizesEqual(apl, mean, "Matrix::AdjustMean");
        ret.push_back( apl - mean );
    }
    return ret;
}

VectorD Matrix::GetCol(int colNum) const
{
    VectorD ret;
    for (int i = 0; i < GetNRows(); ++i)
    {
        ret.Add(at(i).at(colNum));
    }
    return ret;
}
