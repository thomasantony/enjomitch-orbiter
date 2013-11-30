#ifndef OPTIMISERVAR_H
#define OPTIMISERVAR_H

class plan;
class Intercept;
class basefunction;

class OptimiserVar
{
    public:
        OptimiserVar();
        void Init( basefunction * base, Intercept * icept );
        virtual ~OptimiserVar();

        double GetOpti( double * currentValue ) const;
    protected:
    private:

        basefunction * m_base;
        Intercept * m_icept;
};

#endif // OPTIMISERVAR_H
