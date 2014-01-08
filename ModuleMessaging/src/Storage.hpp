#ifndef STORAGE_H
#define STORAGE_H

#include <OrbiterSDK.h>
#include <map>
#include <string>
#include <Util/Result.hpp>

namespace EnjoLib
{
class IMessagingSender;
class Storage
{
    public:
        Storage();
        virtual ~Storage();

        static void Store(const IMessagingSender & sender, const char * varName, bool var);
        static void Store(const IMessagingSender & sender, const char * varName, int var);
        static void Store(const IMessagingSender & sender, const char * varName, double var);
        static void Store(const IMessagingSender & sender, const char * varName, const VECTOR3 & var);
        static void Store(const IMessagingSender & sender, const char * varName, const MATRIX3 & var);
        static void Store(const IMessagingSender & sender, const char * varName, const MATRIX4 & var);
        static void Store(const IMessagingSender & sender, const char * varName, const char * var);

        static Result<bool>     GetBool(    const char * moduleName, const char * varName);
        static Result<int>      GetInt(     const char * moduleName, const char * varName);
        static Result<double>   GetDouble(  const char * moduleName, const char * varName);
        static Result<VECTOR3>  GetVECTOR3( const char * moduleName, const char * varName);
        static Result<MATRIX3>  GetMATRIX3( const char * moduleName, const char * varName);
        static Result<MATRIX4>  GetMATRIX4( const char * moduleName, const char * varName);
        static Result<const char *> GetString(  const char * moduleName, const char * varName);

    protected:
    private:
        static std::string MakeID(const IMessagingSender & sender, const char * varName);
        static std::string MakeID(const char * moduleName, const char * varName);
        template<class T> static Result<T> SearchMap(const char * moduleName, const char * varName, const std::map<std::string, T> & mapToSearch, const T & defaultValue);

        static std::map<std::string, bool> m_bools;
        static std::map<std::string, int> m_ints;
        static std::map<std::string, double> m_doubles;
        static std::map<std::string, VECTOR3> m_vectors;
        static std::map<std::string, MATRIX3> m_matrices3;
        static std::map<std::string, MATRIX4> m_matrices4;
        static std::map<std::string, std::string> m_strings;
};

    template<class T>
    Result<T> Storage::SearchMap(const char * moduleName, const char * varName, const std::map<std::string, T> & mapToSearch, const T & defaultValue)
    {
        const string & id = MakeID(moduleName, varName);
        map<string, T>::const_iterator it = mapToSearch.find(id);
        if(it != mapToSearch.end())
            return Result<T>(it->second, true);
        else
            return Result<T>(defaultValue, false);
    }
}

#endif // STORAGE_H
