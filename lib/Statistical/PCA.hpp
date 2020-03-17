#ifndef PCA_H
#define PCA_H

#include "Matrix.hpp"

namespace EnjoLib
{
class PCA
{
    public:
        PCA(const Matrix & xxx, int numFeaturesToLeave);
        PCA(const std::string & fileNameBase);
        PCA();
        virtual ~PCA();

        Matrix Transform(const Matrix & xxx) const;
        Matrix InverseTransform(const Matrix & xxx) const;

        void Serialize(const std::string & fileNameBase) const;

        Matrix GetFeatureVector() const { return m_featVec; }
        VectorD GetOriginalMean() const { return m_origMean; }

    protected:

    private:
        static const std::string STR_MEAN;
        static const std::string STR_FEAT;
        Matrix m_featVec;
        VectorD m_origMean;
};
}

#endif // PCA_H
