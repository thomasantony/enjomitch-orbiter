#ifndef OPTIMISERBASE_H
#define OPTIMISERBASE_H

#include <vector>
class Intercept;
class basefunction;
struct VarConstraint;
class Optimiser
{
    public:
        Optimiser(basefunction * base, Intercept * icept, const std::vector<VarConstraint> & pArgs2Find);
        Optimiser(basefunction * base, Intercept * icept, VarConstraint pArg2Find);
        virtual ~Optimiser();
        void Optimise() const;

    protected:
    private:
        basefunction * m_base;
        Intercept * m_icept;
        std::vector<VarConstraint> m_pArgs2Find;
};

#endif // OPTIMISERBASE_H
