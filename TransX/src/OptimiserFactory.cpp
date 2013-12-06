#include "OptimiserFactory.h"
#include "Optimiser.h"

OptimiserFactory::OptimiserFactory(basefunction * base, Intercept * icept)
: m_base(base)
, m_icept(icept)
{
}

OptimiserFactory::~OptimiserFactory()
{
}

std::auto_ptr<Optimiser> OptimiserFactory::Create(const std::vector<VarConstraint> & pArgs2Find)
{
    return std::auto_ptr<Optimiser>(new Optimiser(m_base, m_icept, pArgs2Find));
}
std::auto_ptr<Optimiser> OptimiserFactory::Create(VarConstraint pArg2Find)
{
    return std::auto_ptr<Optimiser>(new Optimiser(m_base, m_icept, pArg2Find));
}
std::auto_ptr<Optimiser> OptimiserFactory::CreateDummy()
{
    //return std::auto_ptr<Optimiser>(Create(std::vector<MFDvarfloat*>()));
    return std::auto_ptr<Optimiser>(NULL);
}
