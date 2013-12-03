#ifndef BINSUBJSPHERE_H
#define BINSUBJSPHERE_H

#include <Math/BinSearchOptiSubject.hpp>

class BinSubjSphere : public EnjoLib::BinSearchOptiSubject
{
    public:
        BinSubjSphere(double shift);
        virtual ~BinSubjSphere();

        double UpdateGetValue( double arg );
    protected:
    private:
        double m_shift;
};

#endif // BINSUBJSPHERE_H
