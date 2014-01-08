#include "Storage.hpp"
#include "IMessagingSender.hpp"

using namespace EnjoLib;
using namespace std;
map<string, bool> Storage::m_bools;
map<string, int> Storage::m_ints;
map<string, double> Storage::m_doubles;
map<string, VECTOR3> Storage::m_vectors;
map<string, MATRIX3> Storage::m_matrices3;
map<string, MATRIX4> Storage::m_matrices4;
map<string, string> Storage::m_strings;

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

void Storage::Store(const IMessagingSender & sender, const char * varName, bool var)
{
    m_bools[MakeID(sender, varName)] = var;
}
void Storage::Store(const IMessagingSender & sender, const char * varName, int var)
{
    m_ints[MakeID(sender, varName)] = var;
}
void Storage::Store(const IMessagingSender & sender, const char * varName, double var)
{
    m_doubles[MakeID(sender, varName)] = var;
}
void Storage::Store(const IMessagingSender & sender, const char * varName, const VECTOR3 & var)
{
    m_vectors[MakeID(sender, varName)] = var;
}
void Storage::Store(const IMessagingSender & sender, const char * varName, const MATRIX3 & var)
{
    m_matrices3[MakeID(sender, varName)] = var;
}
void Storage::Store(const IMessagingSender & sender, const char * varName, const MATRIX4 & var)
{
    m_matrices4[MakeID(sender, varName)] = var;
}
void Storage::Store(const IMessagingSender & sender, const char * varName, const char * var)
{
    m_strings[MakeID(sender, varName)] = var;
}

Result<bool> Storage::GetBool(const char * moduleName, const char * varName)
{
    return SearchMap<bool>(moduleName, varName, m_bools, false);
}
Result<int> Storage::GetInt(const char * moduleName, const char * varName)
{
    return SearchMap<int>(moduleName, varName, m_ints, 0);
}
Result<double> Storage::GetDouble(const char * moduleName, const char * varName)
{
    return SearchMap<double>(moduleName, varName, m_doubles, 0);
}
Result<VECTOR3> Storage::GetVECTOR3(const char * moduleName, const char * varName)
{
    return SearchMap<VECTOR3>(moduleName, varName, m_vectors, _V(0,0,0));
}
Result<MATRIX3> Storage::GetMATRIX3(const char * moduleName, const char * varName)
{
    return SearchMap<MATRIX3>(moduleName, varName, m_matrices3, _M(0,0,0,0,0,0,0,0,0));
}
Result<MATRIX4> Storage::GetMATRIX4(const char * moduleName, const char * varName)
{
    MATRIX4 defaultVal = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    return SearchMap<MATRIX4>(moduleName, varName, m_matrices4, defaultVal);
}
Result<const char *> Storage::GetString(const char * moduleName, const char * varName)
{
    const Result<string> & result = SearchMap<string>(moduleName, varName, m_strings, string());
    return Result<const char *>(result.value.c_str(), result.status);
}
