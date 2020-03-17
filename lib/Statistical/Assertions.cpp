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

#include "Assertions.hpp"
#include "VectorTpl.hpp"
#include "Matrix.hpp"

#include <string>
#include <stdexcept>
#include <sstream>

using namespace EnjoLib;

void Assertions::CanMultiply( const Matrix & m1, const Matrix & m2, const char * identifier  )
{
    if ( m1.GetNCols() != m2.GetNRows() )
    {
        std::ostringstream oss;
        oss << "Can't multiply matrices of sizes " << m1.SizeStr() << " and " << m2.SizeStr() << " at\n" << identifier << "\n";
        throw std::invalid_argument( oss.str() );
    }
}
void Assertions::Square( const Matrix & m, const char * identifier )
{
    for (Matrix::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        if ( it->size() != m.size() )
            throw std::invalid_argument( std::string("Matrix not square at\n") + identifier + "()\n");
    }
}
void Assertions::NonEmpty( const Matrix & mat, const char * identifier )
{
    if (mat.empty())
        throw std::invalid_argument( std::string("Empty matrix at\n") + identifier + "()\n");
}
void Assertions::SizesEqual( size_t sz, size_t refSize, const char * identifier )
{
    if ( sz != refSize )
    {
        std::ostringstream oss;
        oss << "Incompatible sizes of: " << sz << " & " << refSize << ", at:\n" << identifier << "()\n";
        throw std::invalid_argument(oss.str());
    }
}

void Assertions::IsTrue( bool cond, const char * identifier )
{
    if ( not cond )
    {
        std::ostringstream oss;
        oss << "Condition is false, at:\n" << identifier << "()\n";
        throw std::invalid_argument(oss.str());
    }
}

void Assertions::IsFalse( bool cond, const char * identifier )
{
    if ( cond )
    {
        std::ostringstream oss;
        oss << "Condition is true, at:\n" << identifier << "()\n";
        throw std::invalid_argument(oss.str());
    }
}

/*
template<class T>
void Assertions::SizesEqual( const VectorTpl<T> & v1, const VectorTpl<T> & v2, const char * identifier ) const
{
    if ( v1.size() != v2.size() )
        throw std::invalid_argument( std::string("Incompatible sizes at\n") + identifier + "()\n");
}

template<class T>
void Assertions::AtLeast2Dimensions( const VectorTpl<T> & v, const char * identifier ) const
{
    if ( v.size() < 2 )
        throw std::invalid_argument( std::string("Dimension must be at least 2\n") + identifier + "()\n");
}
*/
