#ifndef UNIQUEPTR_H
#define UNIQUEPTR_H

#include "PtrHelper.hpp"
#include <memory>
#include <typeinfo>

namespace EnjoLib
{
template <class T, class D=std::default_delete<T> >
class UniquePtr : public std::unique_ptr<T, D>
{
    public:
        using Base = std::unique_ptr<T, D>;
        using Base::unique_ptr;
        using value_type = T;

        const T & operator * () const
        {
            PtrHelper::ThrowIfCondMet(not Base::operator bool(), "UniquePtr", typeid(value_type).name());
            return Base::operator*();
        }

        const T * operator -> () const
        {
            PtrHelper::ThrowIfCondMet(not Base::operator bool(), "UniquePtr", typeid(value_type).name());
            return Base::operator->();
        }

        T & operator * ()
        {
            PtrHelper::ThrowIfCondMet(not Base::operator bool(), "UniquePtr", typeid(value_type).name());
            return Base::operator*();
        }

        T * operator -> ()
        {
            PtrHelper::ThrowIfCondMet(not Base::operator bool(), "UniquePtr", typeid(value_type).name());
            return Base::operator->();
        }

    protected:

    private:
};
}

#endif // UNIQUEPTR_H
