#include "PCAAuto.hpp"
#include "PCA.hpp"
#include "Statistical.hpp"
#include "Assertions.hpp"
#include "EigenAbstract.hpp"
#include "../Math/MaxMinFind.hpp"
#include "../Math/GeneralMath.hpp"

#include <iostream>
#include <map>

using namespace std;
using namespace EnjoLib;

PCAAuto::PCAAuto(){}
PCAAuto::~PCAAuto(){}

#include "../Math/Root/RootSubject.hpp"
#include "../Math/Root/RootFactory.hpp"

class PCAAutoRootSubject : public RootSubject
{
public:
    PCAAutoRootSubject(const EnjoLib::Matrix & xxx, double minQuality);
    double GetRefValue() const override { return m_minQuality; };
    /// Should return the function's value for a given argument.
    double UpdateGetValue( double arg ) override;
    bool IsDiscrete() const override { return true;}

protected:
private:
    double m_minQuality;
    EnjoLib::Matrix m_xxx;
    EnjoLib::Matrix m_xxxT;
    std::map<int, double> m_cache; /// TODO: This could go to a RootSubjectDiscrete class
};

PCAAutoRootSubject::PCAAutoRootSubject(const Matrix & xxx, double minQuality)
: m_minQuality(minQuality)
, m_xxx(xxx)
, m_xxxT(xxx.T())
{}

double PCAAutoRootSubject::UpdateGetValue(double arg)
{
    const int feat2leave = GeneralMath().round(arg);
    //if (m_cache.count(feat2leave))
    {
        //cout << "Cached " << feat2leave << endl;
        //return m_cache[feat2leave];
    }
    const double relMax = PCAAuto::GetQuality(m_xxx, m_xxxT, feat2leave);
    //cout << "Curr quality = " << relMax << ", feat = " << feat2leave << endl;// << ", currMax = " << currMax << ", globMax = " << globMax << endl;
    //m_cache[feat2leave] = relMax;
    return relMax;
}

int PCAAuto::FindMinimalComponents(const Matrix & xxx, const double minQuality) const
{
    double prevRelMax = 0;
    const Matrix & xxxT = xxx.T();
    // Faster is to go from lowest number to highest
    for (int feat2leave = 1; feat2leave < xxx.GetNCols(); ++feat2leave)
    //for (int feat2leave = xxx.GetNCols() - 1; feat2leave >= 1; --feat2leave)
    {
        const double relMax = PCAAuto::GetQuality(xxx, xxxT, feat2leave);
        //cout << "Curr quality = " << relMax << ", feat = " << feat2leave << endl;// << ", currMax = " << currMax << ", globMax = " << globMax << endl;
        if (prevRelMax == 0)
        {
            if (relMax > minQuality)
            {
                return feat2leave; // Could not reach the quality at first shot. Cant decrease.
            }
        }
        else
        {
            if (prevRelMax < minQuality && relMax > minQuality)
            {
                return feat2leave; // Just exceeded the quality. Roll one iteration back.
            }
        }
        prevRelMax = relMax;
    }
    return xxx.GetNCols();
}

double PCAAuto::GetQuality(const Matrix & xxx, const Matrix & xxxT, const int feat2leave)
{
    const PCA pca(xxx, feat2leave);
    const Matrix & xtr = pca.Transform(xxx);
    const Matrix & inv = pca.InverseTransform(xtr);
    const Matrix & invT = inv.T();

    Assertions::SizesEqual(xxxT.size(), invT.size(), "PCAAuto::FindMinimalComponents");
    MaxMinFind<double> maxRMSOneFeature;
    for (size_t row = 0; row < invT.size(); ++row)
    {
        const double rmsFeature = Statistical().RMSTwo(xxxT.at(row), invT.at(row));
        maxRMSOneFeature.UpdateMax(rmsFeature);
    }
    const double currMax = maxRMSOneFeature.GetMax();
    const double relMax = 1 - currMax;

    //cout << "Curr quality = " << relMax << ", feat = " << feat2leave << endl;// << ", currMax = " << currMax << ", globMax = " << globMax << endl;

    return relMax;
}

int PCAAuto::FindMinimalComponentsBinSearch(const Matrix & xxx, const double minQuality, const double minArg) const
{
    PCAAutoRootSubject pcaRS(xxx, minQuality);
    //const double minArg = 1;
    //const double minArg = xxx.GetNCols() / 2;
    unique_ptr<IRootAlgo> rootAlgo = RootFactory::Create(RootType::ROOT_BIN_SEARCH, minArg, xxx.GetNCols(), 1);
    const Result<double> & result = rootAlgo->Run(pcaRS);
    if (result.isSuccess)
    {
        return result.value;
    }
    else
    {
        return 1;
        //return 0;
        //throw std::runtime_error("FindMinimalComponentsBinSearch: Blah");
    }
}



