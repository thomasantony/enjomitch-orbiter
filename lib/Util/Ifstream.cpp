#include "Ifstream.hpp"
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace EnjoLib;

Ifstream::Ifstream(const char* fileName, bool tryOpen)
: ifstream(fileName)
, m_fileName(fileName)
{
    if (tryOpen)
        IsOpenThrow();
}

Ifstream::Ifstream(const std::string& fileName, bool tryOpen)
: ifstream(fileName)
, m_fileName(fileName)
{
    if (tryOpen)
        IsOpenThrow();
}

Ifstream::~Ifstream(){}

void Ifstream::IsOpenThrow() const
{
    if (! is_open())
    {
        throw std::runtime_error("Couldn't open '" + m_fileName + "' for reading!");
    }
}
