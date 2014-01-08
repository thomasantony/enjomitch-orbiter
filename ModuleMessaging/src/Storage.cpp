#include "Storage.hpp"
#include "IMessagingSender.hpp"

using namespace EnjoLib;
using namespace std;
map<string, double> Storage::m_doubles;
map<string, VECTOR3> Storage::m_vectors;

Storage::Storage()
{}

Storage::~Storage()
{}

std::string Storage::MakeID(const IMessagingSender & sender, const char * varName)
{
    return MakeID(sender.GetModuleName(), varName);
}

std::string Storage::MakeID(const char * moduleName, const char * varName)
{
    return std::string() + moduleName + "-" + varName;
}

void Storage::Store(const IMessagingSender & sender, const char * varName, double var)
{
    m_doubles[MakeID(sender, varName)] = var;
}

void Storage::Store(const IMessagingSender & sender, const char * varName, const VECTOR3 & var)
{
    m_vectors[MakeID(sender, varName)] = var;
}

Result<double> Storage::GetDouble(const char * moduleName, const char * varName)
{
    const string & id = MakeID(moduleName, varName);
    map<string, double>::iterator it = m_doubles.find(id);
    if(it != m_doubles.end())
        return Result<double>(it->second, true);
    else
        return Result<double>(0, false);
}

Result<VECTOR3> Storage::GetVECTOR3(const char * moduleName, const char * varName)
{
    const string & id = MakeID(moduleName, varName);
    map<string, VECTOR3>::iterator it = m_vectors.find(id);
    if(it != m_vectors.end())
        return Result<VECTOR3>(it->second, true);
    else
        return Result<VECTOR3>(_V(0,0,0), false);
}


