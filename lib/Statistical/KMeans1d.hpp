#ifndef KMeans1d_H
#define KMeans1d_H

//#include "VectorF.hpp"
#include <vector>

namespace EnjoLib
{
class KMeans1dCluster;

class KMeans1d
{
    public:
        KMeans1d(bool clustersSorted = false);
        virtual ~KMeans1d();

        void AddObservation(float obs);
        std::vector<float> Reduce(int reductions) const;
        std::vector<KMeans1dCluster> ReduceGetClusters(int reductions) const;

        static float Dist(float p, float c);

    protected:

    private:
        bool m_clustersSorted = false;
        std::vector<float> m_obs;
};

}
#endif // KMeans1d_H
