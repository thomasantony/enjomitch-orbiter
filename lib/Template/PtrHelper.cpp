#include "PtrHelper.hpp"

#include <stdexcept>
#include <string>

PtrHelper::PtrHelper()
{
    //ctor
}

PtrHelper::~PtrHelper()
{
    //dtor
}

void PtrHelper::ThrowIfCondMet(bool condition, const char * typeName, const char * subTypeName)
{
    if (condition)
    {
        const std::string msg = std::string(typeName) + "<" + subTypeName + ">: Not initialized!";
        throw std::runtime_error(msg);
    }
}
