#ifndef KMEANS1DCLUSTER_H
#define KMEANS1DCLUSTER_H

#include <vector>

namespace EnjoLib
{
class KMeans1dCluster
{
    public:
        KMeans1dCluster(){}
        KMeans1dCluster(const std::vector<float> & obs, float val) : level(val), obs(obs) {}
        virtual ~KMeans1dCluster();
        void Reset();
        void Assign(int pid);
        void Recalc();
        bool IsReassigned() const;
        float GetLevel() const { return level; }
        std::vector<float> GetMembersLevels(bool sorted = false) const;

        bool operator < (const KMeans1dCluster & other) const;
    private:

        float level;
        std::vector<int> idPoints;
        std::vector<int> idPointsPrev;
        std::vector<float> obs;
};
}

#endif // KMEANS1DCLUSTER_H
