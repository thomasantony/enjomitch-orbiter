#ifndef CROSSVALIDATION_H
#define CROSSVALIDATION_H

namespace EnjoLib
{
class CrossValidation
{
    public:
        CrossValidation(unsigned numCVs, unsigned szData);
        virtual ~CrossValidation();

        enum Algo
        {
            STRATIFIED,
            SHUFFLED
        };

        bool IsTrain(int icv, int i, Algo algo) const;

        bool IsTrainStratified(int icv, int i) const;
        bool IsTrainShuffled(int icv, int i) const;

    protected:

    private:

        unsigned m_numCVs = 3;
        unsigned m_szData = 0;
};
}

#endif // CROSSVALIDATION_H
