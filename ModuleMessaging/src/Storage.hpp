#ifndef STORAGE_H
#define STORAGE_H

#include <OrbiterSDK.h>
#include <map>
#include <string>
#include "Result.hpp"

namespace EnjoLib
{
class IMessagingSender;
class Storage
{
    public:
        Storage();
        virtual ~Storage();

        static void Store(const IMessagingSender & sender, const char * varName, double var);
        static void Store(const IMessagingSender & sender, const char * varName, const VECTOR3 & var);

        static Result<double> GetDouble(const char * moduleName, const char * varName);
        static Result<VECTOR3> GetVECTOR3(const char * moduleName, const char * varName);

    protected:
    private:
        static std::string MakeID(const IMessagingSender & sender, const char * varName);
        static std::string MakeID(const char * moduleName, const char * varName);

        static std::map<std::string, double> m_doubles;
        static std::map<std::string, VECTOR3> m_vectors;
};
}

#endif // STORAGE_H
