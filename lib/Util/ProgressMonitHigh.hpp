#ifndef PROGRESSMONITHIGH_H
#define PROGRESSMONITHIGH_H

#include <string>
#include "Timer.hpp"

namespace EnjoLib
{
class ProgressMonitHigh
{
    public:
        ProgressMonitHigh(int numBars = 25);
        virtual ~ProgressMonitHigh();
        void PrintProgressBarTime(size_t i, size_t sz, const std::string & id = std::string(), bool onPercentChange = true) const;
        void Reset();

    private:
        Timer m_timer;
        mutable int m_prevStringSize = 0;
        mutable int m_prevPercentDone = -1;
        int m_numBars;
};
}

#endif // PROGRESSMONITHIGH_H
