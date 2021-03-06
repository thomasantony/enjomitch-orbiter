#ifndef OPTIFUNCTION_H
#define OPTIFUNCTION_H

//#include <Math/BinSearchOptiSubject.hpp>
#include <Math/Opti/OptiSubject.hpp>
#include <Math/Opti/OptiMultiSubject.hpp>
#include "VarConstraint.h"
class basefunction;
class Intercept;

// Base for the "Optimisation Problem". Could be used for multiple dimension optimisation
class OptiFunctionBase
{
    public:
        OptiFunctionBase(basefunction * base, Intercept * icept)
           : m_base(base)
           , m_icept(icept)
        {}
    protected:
        double RecalculateGetValue();
        basefunction * m_base;

    private:

        Intercept * m_icept;
};

// Defines the one dimensional "Optimisation Problem"
class OptiFunction : public OptiFunctionBase, public EnjoLib::OptiSubject
{
    public:
        OptiFunction(VarConstraint toOpti, basefunction * base, Intercept * icept)
        : OptiFunctionBase(base, icept)
        , m_toOpti(toOpti)
        {
        }
        double UpdateGetValue( double arg );
    private:
        VarConstraint m_toOpti;
};

class OptiMultiFunction : public OptiFunctionBase, public EnjoLib::OptiMultiSubject
{
    public:
        OptiMultiFunction(std::vector<VarConstraint> toOpti, basefunction * base, Intercept * icept)
        : OptiFunctionBase(base, icept)
        , m_toOpti(toOpti)
        {
        }
        double Get(const double * in, int n);

        std::vector<double> GetStart() const;
        std::vector<double> GetStep() const;

    private:
        std::vector<VarConstraint> m_toOpti;
};

#endif // OPTIFUNCTION_H
