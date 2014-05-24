#ifndef BOUNDSEARCH_H
#define BOUNDSEARCH_H
#include "../Util/Result.hpp"
#include "BinSearchOptiSubject.hpp"

namespace EnjoLib
{
class BoundSearch
{
    public:
        BoundSearch();
        virtual ~BoundSearch();

        Result<double> FindUpper(BinSearchOptiSubject & subj, double start, double minIncrement = 1, double maxIncrement = 0);
        Result<double> FindLower(BinSearchOptiSubject & subj, double start, double minIncrement = 1, double maxIncrement = 0);
        Result<double> Find(BinSearchOptiSubject & subj, double start, bool upper, double minIncrement = 1, double maxIncrement = 0);
    protected:
    private:
};
}
#endif // BOUNDSEARCH_H
