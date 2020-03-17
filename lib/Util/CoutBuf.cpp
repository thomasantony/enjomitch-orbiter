#include "CoutBuf.hpp"
#include <iostream>

using namespace std;
using namespace EnjoLib;

std::ostream cnull(0);
std::wostream wcnull(0);

std::ostream & GetLog(bool verbose)
{
    if (verbose)
        return std::cout;
    else
        //return std::cout;
        return cnull;
}


Log::Log()
: m_cnull(0)
, m_wcnull(0)
{
}

std::ostream & Log::GetLog(bool verbose)
{
    if (verbose)
        return std::cout;
    else
        //return std::cout;
        return m_cnull;
}
