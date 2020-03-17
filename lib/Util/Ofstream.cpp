#include "Ofstream.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace EnjoLib;

Ofstream::Ofstream(const char* fileName, bool tryOpen)
: ofstream(fileName)
, m_fileName(fileName)
{
    if (tryOpen)
        IsOpenThrow();
}

Ofstream::Ofstream(const std::string& fileName, bool tryOpen)
: ofstream(fileName)
, m_fileName(fileName)
{
    if (tryOpen)
        IsOpenThrow();
}

Ofstream::~Ofstream(){}

void Ofstream::IsOpenThrow() const
{
    if (! is_open())
    {
        throw std::runtime_error("Couldn't open '" + m_fileName + "' for writing!");
    }
    else
    {
        //std::cout << "Opened '" + m_fileName + "' for writing.\n";
    }
}
