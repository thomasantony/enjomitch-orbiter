#include "MFDTextCalculator.hpp"

using namespace EnjoLib::MFDGoodies;

const int MFDTextCalculator::m_cNumLines = 24;
const int MFDTextCalculator::m_cCharLen = 35;

MFDTextCalculator::MFDTextCalculator(DWORD w, DWORD h)
    : m_lineSpacing(w/m_cNumLines), m_charWidth(h/m_cCharLen)
{
}

int MFDTextCalculator::X( int pos )
{
    return pos * m_charWidth;
}

int MFDTextCalculator::Y( int pos )
{
    return pos * m_lineSpacing;
}


