#ifndef EIGENFACTORYABSTRACT_H
#define EIGENFACTORYABSTRACT_H

#include "../Template/UniquePtr.hpp"

namespace EnjoLib
{
class EigenAbstract;
class EigenFactoryAbstract
{
    public:
        EigenFactoryAbstract();
        virtual ~EigenFactoryAbstract();

        virtual UniquePtr<EigenAbstract> CreateEigen() const = 0;

    protected:

    private:
};
}

#endif // EIGENFACTORYABSTRACT_H
