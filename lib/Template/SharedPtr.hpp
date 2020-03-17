#ifndef SHAREDPTR_H
#define SHAREDPTR_H

#include "PtrHelper.hpp"
#include <memory>
#include <typeinfo>

namespace EnjoLib
{
template <class T>
class SharedPtr : public std::shared_ptr<T>
{
    public:
        using Base = std::shared_ptr<T>;
        using Base::shared_ptr;
        using value_type = T;

        const T & operator * () const
        {
            PtrHelper::ThrowIfCondMet(not Base::operator bool(), "SharedPtr", typeid(value_type).name());
            return Base::operator*();
        }

        const T * operator -> () const
        {
            PtrHelper::ThrowIfCondMet(not Base::operator bool(), "SharedPtr", typeid(value_type).name());
            return Base::operator->();
        }

    protected:

    private:
};
}

#endif // SHAREDPTR_H
