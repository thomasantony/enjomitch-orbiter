#define STRICT
#define ORBITER_MODULE

#include <OrbiterSdk.h>
#include "ModuleMessaging.hpp"
#include "Storage.hpp"

using namespace EnjoLib;

ModuleMessaging::ModuleMessaging()
{}

ModuleMessaging::~ModuleMessaging()
{}

Result<bool> ModuleMessaging::GetBool(const char * moduleName, const char * varName)
{
    return Storage::GetBool(moduleName, varName);
}
Result<int> ModuleMessaging::GetInt(const char * moduleName, const char * varName)
{
    return Storage::GetInt(moduleName, varName);
}
Result<double> ModuleMessaging::GetDouble(const char * moduleName, const char * varName)
{
    return Storage::GetDouble(moduleName, varName);
}
Result<VECTOR3> ModuleMessaging::GetVECTOR3(const char * moduleName, const char * varName)
{
    return Storage::GetVECTOR3(moduleName, varName);
}
Result<MATRIX3> ModuleMessaging::GetMATRIX3(const char * moduleName, const char * varName)
{
    return Storage::GetMATRIX3(moduleName, varName);
}
Result<MATRIX4> ModuleMessaging::GetMATRIX4(const char * moduleName, const char * varName)
{
    return Storage::GetMATRIX4(moduleName, varName);
}
Result<const char *> ModuleMessaging::GetString(const char * moduleName, const char * varName)
{
    return Storage::GetString(moduleName, varName);
}
