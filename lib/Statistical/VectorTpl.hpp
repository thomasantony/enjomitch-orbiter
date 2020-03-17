#ifndef VECTORTPL_H
#define VECTORTPL_H


#include <vector>
#include <string>
namespace EnjoLib
{
template<class T>
class VectorTpl : public std::vector<T>
{
    public:
        std::string Print() const; // TODO
        std::string PrintScilab( const char * varName ) const;
        std::string PrintPython( const char * varName ) const;

        VectorTpl( const std::vector<T> & init );
        VectorTpl( const std::vector<bool> & init );
        VectorTpl( int n );
        VectorTpl();
        virtual ~VectorTpl();

        void Add(const T & val);

        //! Length of vector
        T Len() const;
        //! Normalised copy of vector
        VectorTpl Norm() const;
        T SumSquares() const;
        T SumAbs() const;
        T Sum() const;
        T Mean() const;
        T MeanWeighted() const;
        T Max() const;
        T Min() const;
        VectorTpl AdjustMean() const;
        VectorTpl Slice  (unsigned idx, unsigned len) const;
        VectorTpl SliceTS(unsigned idx, unsigned len) const;
        VectorTpl Diffs() const;

        VectorTpl & operator += (const VectorTpl & par);
        VectorTpl & operator -= (const VectorTpl & par);
        VectorTpl & operator /= (const T f);
        VectorTpl & operator *= (const T f);
        VectorTpl & operator += (const T f);
        VectorTpl & operator -= (const T f);

        VectorTpl operator + (const VectorTpl & par) const;
        VectorTpl operator - (const VectorTpl & par) const;
        VectorTpl operator - () const;
        VectorTpl operator + (const T f) const;
        VectorTpl operator - (const T f) const;
        VectorTpl operator * (const T f) const;
        VectorTpl operator / (const T f) const;
        bool operator > (const VectorTpl & par) const;
        bool operator < (const VectorTpl & par) const;



    protected:
    private:
        void SizesEqual( const VectorTpl & par, const char * functionName ) const;

        typedef typename std::vector<T>::const_iterator CIt;
        typedef typename std::vector<T>::iterator It;
};
}

#endif // VECTORTPL_H
