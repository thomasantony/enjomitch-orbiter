#ifndef BINSUBJSPHERE_H
#define BINSUBJSPHERE_H

#include <Math/BinSearchOptiSubject.hpp>
#include <Statistical/Vector.hpp>

class BinSubjSphere : public EnjoLib::BinSearchOptiSubject
{
    public:
        BinSubjSphere(const EnjoLib::Vector & shift);
        virtual ~BinSubjSphere();

        double UpdateGetValue( const EnjoLib::Vector & arg );
    protected:
    private:
        EnjoLib::Vector m_shift;
};

#endif // BINSUBJSPHERE_H
