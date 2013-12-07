#ifndef BINSUBJSPHERE2D_H
#define BINSUBJSPHERE2D_H

#include <Math/BinSearchOpti2DSubject.hpp>
#include <Systems/Point.hpp>
class BinSubjSphere2D : public EnjoLib::BinSearchOpti2DSubject
{
    public:
        BinSubjSphere2D(EnjoLib::Point shift);
        virtual ~BinSubjSphere2D();

        double UpdateGetValue( const EnjoLib::Point & arg );
    protected:
    private:

        EnjoLib::Point m_shift;
};

#endif // BINSUBJSPHERE2D_H
