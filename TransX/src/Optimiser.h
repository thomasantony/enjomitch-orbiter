#ifndef OPTIMISERBASE_H
#define OPTIMISERBASE_H

#include <vector>
class Intercept;
class basefunction;
class MFDvarfloat;
class Optimiser
{
    public:
        Optimiser(basefunction * base, Intercept * icept, const std::vector<MFDvarfloat*> & pArgs2Find);
        Optimiser(basefunction * base, Intercept * icept, MFDvarfloat* pArg2Find);
        virtual ~Optimiser();
        void Optimise( double * toOpti ) const;

    protected:
    private:
        basefunction * m_base;
        Intercept * m_icept;
        std::vector<MFDvarfloat*> m_pArgs2Find;
        static const double m_cdefaultMax;
        static const double m_cdefaultMin;
        static const double m_cdefaultRatioHohmann;
};

#endif // OPTIMISERBASE_H
