#include "OptiFactory.hpp"
#include <stdexcept>
#include "OptiBinSearch.hpp"
#include "OptiBrent.hpp"

using namespace EnjoLib;

OptiFactory::OptiFactory(){}
OptiFactory::~OptiFactory(){}

std::auto_ptr<IOptiAlgo> OptiFactory::Create(OptiType type, double minArg, double maxArg, double eps)
{
    switch (type)
    {
    case BIN_SEARCH:
        return std::auto_ptr<IOptiAlgo>(new OptiBinSearch(minArg, maxArg, eps));
    case BRENT:
        return std::auto_ptr<IOptiAlgo>(new OptiBrent(minArg, maxArg, eps));
    }

    throw std::invalid_argument("Not handled OptiType");
}
