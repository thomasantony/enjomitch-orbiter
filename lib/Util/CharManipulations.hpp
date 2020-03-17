/*
Modified BSD License

This file originates from:
http://sf.net/projects/enjomitchsorbit

Copyright (c) 2002, David "Daver" Rowbotham
Copyright (c) 2011, Szymon "Enjo" Ender
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

#ifndef CHAR_MANIPULATIONS_HPP_INCLUDED
#define CHAR_MANIPULATIONS_HPP_INCLUDED

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace EnjoLib
{
class CharManipulations
{
public:
	/// Author - David "Daver" Rowbotham
	//void char_replace_s(char *src, char lookfor, char replacewith, size_t maxlength);
	/// Author - Szymon Ender
	std::string Replace(const std::string & in, const std::string & lookFor, const std::string & replaceWith ) const;
	std::string Trim(const std::string & in ) const;
	std::string ToStr(double d, unsigned precision = 2) const; // Don't use!
	std::string ToStr(int i) const;
	std::string MakeLeadingZeroes(int d, unsigned numZeroes) const;
	bool StartsWith(const std::string & str, const std::string & prefix) const;
	bool Contains(const std::string & str, const std::string & toFind) const;
	bool EndsWith(const std::string & str, const std::string & sufffix) const;
	std::vector<std::string> EndsWith(const std::vector<std::string> & files, const std::string & sufffix) const;
	std::pair<std::string, std::string> GetFileNameExtension(const std::string & fileWithExt) const;
	bool ToDouble(const std::string & in, double * d) const;
	double ToDouble(const std::string & in) const;
	int ToInt(const std::string & in) const;
	bool ToInt(const std::string & in, int * i) const;
	template <class T> bool ToNumber(const std::string & in, T * d) const;
	template <class T> T    ToNumber(const std::string & in) const;
	template <class T> std::vector<T> ToNumbersVec(const std::vector<std::string> & in) const;
	std::string ToUpper(std::string in) const;
	std::string ToLower(std::string in) const;

private:
};

template <class T> bool CharManipulations::ToNumber(const std::string & in, T * number) const
{
    *number = 0;
    std::istringstream ss;
    ss.str(Trim(in));
    if ( ! (ss >> *number) )
        return false;
    return true;
}

template <class T> T CharManipulations::ToNumber(const std::string & in) const
{
    T number;
    if (!ToNumber<T>(in, &number))
        throw std::invalid_argument("Not number: '" + in + "'");
    return number;
}

template <class T> std::vector<T> CharManipulations::ToNumbersVec(const std::vector<std::string> & in) const
{
    std::vector<T> ret;
    for (size_t i = 0; i < in.size(); ++i)
    {
        const T & number = ToNumber<T>(in.at(i));
        ret.push_back(number);
    }
    return ret;
}

}

#endif
