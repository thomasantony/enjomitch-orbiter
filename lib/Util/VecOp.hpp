#ifndef VECOP_H
#define VECOP_H

#include <vector>

namespace EnjoLib
{

class VecOp
{
    public:
        template <class T> std::vector<T> Add(const std::vector<T> & one, const std::vector<T> & two) const;
        template <class T> std::vector<T> AddConstMem(const std::vector<T> & one, const std::vector<T> & two) const;
        template <class T> void AddRef(const std::vector<T> & one, std::vector<T> * two) const;
        template <class T> int FindMaxIdx(const std::vector<T> & v) const;
        template <class T> int FindMinIdx(const std::vector<T> & v) const;
    protected:
    private:
};

template <class T>
std::vector<T> VecOp::Add(const std::vector<T> & a, const std::vector<T> & b) const
{
    std::vector<T> ret(a);
    ret.insert(ret.end(), b.begin(), b.end());
    return ret;
}

template <class T>
std::vector<T> VecOp::AddConstMem(const std::vector<T> & a, const std::vector<T> & b) const
{
    std::vector<T> ret(a);
    std::copy (b.begin(), b.end(), std::back_inserter(ret));
    return ret;
}

template <class T>
void VecOp::AddRef(const std::vector<T> & a, std::vector<T> * two) const
{
    std::copy (a.begin(), a.end(), std::back_inserter(*two));
}

template <class T>
int VecOp::FindMaxIdx(const std::vector<T> & v) const
{
    T mv = -1;
    int mi = -1;
    for (unsigned i = 0; i < v.size(); ++i)
    {
        const T & val = v.at(i);
        if (mi == -1 || val > mv)
        {
            mi = i;
            mv = val;
        }
    }
    return mi;
}

template <class T>
int VecOp::FindMinIdx(const std::vector<T> & v) const
{
    T mv = -1;
    int mi = -1;
    for (unsigned i = 0; i < v.size(); ++i)
    {
        const T & val = v.at(i);
        if (mi == -1 || val < mv)
        {
            mi = i;
            mv = val;
        }
    }
    return mi;
}


}
#endif // VECOP_H
