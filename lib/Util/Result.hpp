#ifndef BINSEARCHRESULT_H
#define BINSEARCHRESULT_H

namespace EnjoLib
{
template <class T>
class Result
{
    public:
        Result( const T & value, bool status );

        T value;
        bool status;
};

template <class T>
Result<T>::Result( const T & value, bool status )
{
    this->value = value;
    this->status = status;
}
}

#endif // BINSEARCHRESULT_H
