#ifndef MFDTextCalculator_H
#define MFDTextCalculator_H

#include "MyDC.h"

namespace EnjoLib
{
namespace MFDGoodies
{
class MFDTextCalculator
{
    public:
        MFDTextCalculator(DWORD widthMFD, DWORD heightMFD);
        int X( int pos );
        int Y( int pos );
    protected:
    private:

        const int m_lineSpacing;
        const int m_charWidth;
        static const int m_cNumLines;
        static const int m_cCharLen;
};
}
}


#endif // MFDTextCalculator_H
