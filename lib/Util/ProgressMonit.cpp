#include "ProgressMonit.hpp"
#include <sstream>
#include <iostream>

using namespace std;
using namespace EnjoLib;

ProgressMonit::ProgressMonit(int numBars) : m_numBars(numBars){}
ProgressMonit::~ProgressMonit(){}

int ProgressMonit::GetPercentDoneInt(size_t i, size_t sz) const
{
    int percentDone = int(i / float(sz) * 100 * 100) / 100;
    return percentDone;
}

std::string ProgressMonit::GetProgressBar(size_t i, size_t sz) const
{
    int percentDone = GetPercentDoneInt(i, sz);
    const float numTick = m_numBars / 100.0;
    const int currentTick = percentDone * numTick;
    ostringstream oss;
    for (int j = 0; j < m_numBars; ++j)
        oss << (j < currentTick ? "=" : ".");
    return oss.str();
}

void ProgressMonit::PrintPercentDone(size_t i, size_t sz) const
{
    int percentPrev = int(i / (float)sz * 10);
    int percent = int((i+1) / (float)sz * 10);
    if (percent != percentPrev || i == 0)
    {
        std::cout << percent * 10 << "% ";
        std::cout.flush();
    }

    if (i == sz - 1)
        std::cout << "  Done!" << std::endl;
}

double ProgressMonit::GetTimeLeft(size_t i, size_t sz, double elapsedSeconds) const
{
    if (i == 0)
        return 0;
    double percentDone = i / double(sz);
    double avgSpeed = percentDone / elapsedSeconds;
    double leftPercent = 1 - percentDone;
    double leftTime = leftPercent / avgSpeed;
    return leftTime;
}
