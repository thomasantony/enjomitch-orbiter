#include "PCA.hpp"
#include "Statistical.hpp"
#include "EigenAbstract.hpp"

#include "../Util/Ofstream.hpp"
#include "../Util/Tokenizer.hpp"
#include "../Util/CharManipulations.hpp"

#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace EnjoLib;

const std::string PCA::STR_MEAN = "-mean";
const std::string PCA::STR_FEAT = "-feat";

PCA::~PCA(){}
PCA:: PCA(){}
PCA:: PCA(const Matrix & xxx, int numFeaturesToLeave)
{
    const Statistical stat;
    const std::unique_ptr<EigenAbstract> eigen = EigenAbstract::CreateDefault();
    const Matrix & covMat = stat.CovarianceMatrix(xxx);
    m_origMean = stat.MeanCols(xxx);
    m_featVec = eigen->GetSortedFeatureVectorNumber(covMat, numFeaturesToLeave);
}

PCA:: PCA(const std::string & fileNameBase)
{
    const CharManipulations cman;
    const Tokenizer tok;
    std::ostringstream oss;
    string lineMean;
    try
    {
        lineMean = tok.GetLines(fileNameBase + STR_MEAN).at(0);
        m_origMean = cman.ToNumbersVec<double>(tok.Tokenize(lineMean));
    }
    catch (std::exception & ex)
    {
        oss << "PCA:: PCA() Mean: " << ex.what() << ", lineMean = " << lineMean << endl;
        throw std::runtime_error(oss.str());
    }
    try
    {
        const vector<string> & linesFeat = tok.GetLines(fileNameBase + STR_FEAT);
        for (const string & lineFeat : linesFeat)
        {
            if (lineFeat.empty())
            {
                continue;
            }
            const VectorD & feat = cman.ToNumbersVec<double>(tok.Tokenize(lineFeat));
            m_featVec.push_back(feat);
        }
    }
    catch (std::exception & ex)
    {
        oss << "PCA:: PCA() Feat: " << ex.what() << endl;
        throw std::runtime_error(oss.str());
    }
}

Matrix PCA::Transform(const Matrix & xxx) const
{
    //cout << "Transform " << xxx.SizeStr() << endl;
    //cout << "Transform " << m_featVec.Print() << endl;
    const Matrix & xadjMean  = xxx.AdjustMeanCols(m_origMean);
    const Matrix & xadjMeanT = xadjMean.T();
    const Matrix & ret = m_featVec * xadjMeanT;
    return ret.T();
}

Matrix PCA::InverseTransform(const Matrix & xxx) const
{
    const Matrix & mul = m_featVec.T() * xxx.T();
    const Matrix & ret = mul.ApplyMeanRows(m_origMean);
    return ret.T();
}

void PCA::Serialize(const std::string & fileNameBase) const
{
    Ofstream fmean(fileNameBase + STR_MEAN);
    Ofstream ffeat(fileNameBase + STR_FEAT);
    fmean << m_origMean.Print() << endl;
    ffeat << m_featVec.Print() << endl;
}
