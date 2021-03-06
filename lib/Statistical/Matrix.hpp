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

#ifndef MATRIX_H
#define MATRIX_H

#include "VectorD.hpp"
namespace EnjoLib
{
class Matrix : public std::vector<VectorD>
{
    public:
        std::string Print() const;
        std::string PrintScilab( const char * varName ) const;
        std::string SizeStr() const;
        Matrix();
        Matrix(int n);
        Matrix(int n, int m);
        virtual ~Matrix();

        void Add(const VectorD & vec);

        Matrix T() const;
        Matrix & TMe();
        Matrix AdjustMeanRows() const;
        Matrix AdjustMeanCols() const;
        Matrix AdjustMeanCols(const VectorD & mean) const;
        Matrix ApplyMeanCols(const VectorD & mean) const;
        Matrix ApplyMeanRows(const VectorD & mean) const;
        VectorD Flatten() const;
        VectorD GetCol(int colNum) const;
        int GetNRows() const;
        int GetNCols() const
        {
            if ( empty() )
                return 0;
            return this->operator[](0).size();
        }
        Matrix FilterByMask(const std::vector<bool> & mask) const;
        Matrix & FilterByMaskMe(const std::vector<bool> & mask);

        Matrix operator * (const Matrix & par) const;

    protected:
    private:
        Matrix ApplyMean(const Matrix & mat2Apply, const VectorD & mean) const;
        Matrix AdjustMean(const Matrix & mat2Apply) const;
};
}
#endif // MATRIX_H
