#include "CrossValidation.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;
using namespace EnjoLib;

CrossValidation::~CrossValidation(){}
CrossValidation::CrossValidation(unsigned numCVs, unsigned szData)
: m_numCVs(numCVs)
, m_szData(szData)
{
}

bool CrossValidation::IsTrainStratified(int icv, int i) const
{
    int jcv = i % m_numCVs;
    if (jcv == icv)
    {
        //cout << i << " jcv : " << jcv << " icv: " << icv << " : Adding test\n";
        return false;
    }
    else
    {
        //cout << i << " jcv : " << jcv << " icv: " << icv << " : Adding train\n";
        return true;
    }
}

bool CrossValidation::IsTrainShuffled(int icv, int i) const
{
    const int itestMin = (icv + 0) * m_szData / float(m_numCVs);
    const int itestMax = (icv + 1) * m_szData / float(m_numCVs);
    //cout << "itestMin = " << itestMin << ", itestMax = " << itestMax << ", szz = " << m_szData << endl;
    if (itestMin <= i && i < itestMax)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool CrossValidation::IsTrain(int icv, int i, CrossValidation::Algo algo) const
{
    switch (algo)
    {
    case STRATIFIED:
        return IsTrainStratified(icv, i);
    case SHUFFLED:
        return IsTrainShuffled(icv, i);
    }
    throw std::runtime_error("CrossValidation::IsTrain(): Not handled " + std::to_string(algo));
}
