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

Result<double> ModuleMessaging::GetDouble(const char * moduleName, const char * varName)
{
    return Storage::GetDouble(moduleName, varName);
}
Result<VECTOR3> ModuleMessaging::GetVECTOR3(const char * moduleName, const char * varName)
{
    return Storage::GetVECTOR3(moduleName, varName);
}

