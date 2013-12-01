#ifndef OPTIMISERBASE_H
#define OPTIMISERBASE_H

#include <vector>
#include "mfdvartypes.h"
class Intercept;
class basefunction;

class Optimiser
{
    public:
        Optimiser(basefunction * base, Intercept * icept, const std::vector<MFDvarfloat*> & pArgs2Find);
        Optimiser(basefunction * base, Intercept * icept, MFDvarfloat* pArg2Find);
        virtual ~Optimiser();
        void Optimise() const;

    protected:
    private:
        basefunction * m_base;
        Intercept * m_icept;
        std::vector<MFDvarfloat*> m_pArgs2Find;
};

#endif // OPTIMISERBASE_H
