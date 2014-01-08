#ifndef MESSAGINGMODULE_H
#define MESSAGINGMODULE_H

#include <OrbiterSDK.h>

#include <string>
#include <Util/Result.hpp>

namespace EnjoLib
{
class __declspec(dllexport) ModuleMessaging
{
    public:
        ModuleMessaging();
        virtual ~ModuleMessaging();

        Result<double> GetDouble(const char * moduleName, const char * varName);
        Result<VECTOR3> GetVECTOR3(const char * moduleName, const char * varName);

    protected:
    private:
};
}

#endif // MESSAGINGMODULE_H
