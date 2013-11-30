#ifndef OPTIMISERVAR_H
#define OPTIMISERVAR_H

class basefunction;

class OptimiserVar
{
    public:
        OptimiserVar();
        void Init( basefunction * baseFunc );
        virtual ~OptimiserVar();

        double GetOpti() {return 1;}
    protected:
    private:

        basefunction * m_baseFunc;
};

#endif // OPTIMISERVAR_H
