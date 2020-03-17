#ifndef TOOLSMIXED_H
#define TOOLSMIXED_H

#include <string>
#include <map>

namespace EnjoLib
{
class ToolsMixed
{
    public:
        ToolsMixed(){}
        virtual ~ToolsMixed(){}

        std::string BinHex2Str(const unsigned char* data, int len) const;
        void AnimationPropeller(int * idx) const;
        void Animation09AZ(int * idx) const;
        void AnimationCustom(int * idx, const std::string & animSeries) const;
        std::map<std::string, std::string> FromPythonDict(const std::string & dictStr) const;
        void SystemCallWarn(const std::string & command, const std::string & functionName) const;
        bool SystemCallWarnBool(const std::string & command, const std::string & functionName) const;
        void SystemCallThrow(const std::string & command, const std::string & functionName) const;

    protected:

    private:
        std::string SystemCallPrepareMessage(const std::string & command, const std::string & functionName, int err) const;
};
}
#endif // TOOLSMIXED_H
