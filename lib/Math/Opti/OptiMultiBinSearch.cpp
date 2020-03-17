#include "OptiMultiBinSearch.hpp"
#include "OptiMultiSubject.hpp"
#include <Math/MultiDimIter/MultiDimIterTpl.hpp>
#include <Statistical/VectorD.hpp>
#include <cstddef>
#include <iostream>

using namespace EnjoLib;
using namespace std;

OptiMultiBinSearch::OptiMultiBinSearch()
: m_subj(NULL)
{
    //ctor
}

OptiMultiBinSearch::~OptiMultiBinSearch()
{
    //dtor
}

Result<std::vector<double> > OptiMultiBinSearch::Run( OptiMultiSubject & subj, int numSlices, double eps) const
{
    m_subj = &subj;
    const std::vector<double> & start = subj.GetStart();
    const std::vector<OptiMultiSubject::Bounds> & bounds = subj.GetBounds();
    m_vopt = std::vector<double>(bounds.size());

    std::vector<OptiMultiSubject::Bounds> newBounds = bounds;
    //for (unsigned dim = 0; dim < bounds.size())
    MultiDimIterTpl<double>::VVt data;
    do
    {
        MultiDimIterTpl<double> iter;
        data.clear();
        for (const OptiMultiSubject::Bounds & b : newBounds)
        {
            std::vector<double> spaceElements;
            const double len = b.max - b.min;
            const double delta = len / double(numSlices + 1);
            for (int i = 0; i <= numSlices + 1; ++i)
            {
                const double element = b.min + i * delta;
                spaceElements.push_back(element);
            }
            data.push_back(spaceElements);
        }

        iter.StartIteration(data, *this);
        //numSlices *= 2;
        //numSlices++;
        //MultiDimIterTpl<double>::VVt dataNew;

        std::vector<OptiMultiSubject::Bounds> newNewBounds;
        //for (const OptiMultiSubject::Bounds & b : newBounds)
        for (unsigned dim = 0; dim < newBounds.size(); ++dim)
        {
            const OptiMultiSubject::Bounds & b = newBounds.at(dim);
            std::vector<double> spaceElements;
            const double len = b.max - b.min;
            const double delta = len / double(numSlices + 1);

            double min = m_vopt.at(dim) - delta;
            double max = m_vopt.at(dim) + delta;
            OptiMultiSubject::Bounds nb(min, max);
            newNewBounds.push_back(nb);

            //cout << "min, max = " << nb.min << ", " << nb.max << endl;
        }
        //cout << endl;
        newBounds = newNewBounds;
    } while(m_epsReached > eps);



    return Result<std::vector<double> >(m_vopt, false);
}

void OptiMultiBinSearch::Consume(const std::vector<double> & data) const
{
    double val = m_subj->Get(data.data(), data.size());

    if (m_yopt == 0 || val < m_yopt)
    {
        m_yopt = val;
        VectorD vdata(data);
        VectorD vopt(m_vopt);
        double len = (vdata-vopt).Len();
        m_epsReached = len;
        m_vopt = data;
        //cout << "min = " << val << endl;
    }
}

