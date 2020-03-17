#include "ToolsMixed.hpp"
#include "Tokenizer.hpp"
#include "CharManipulations.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace EnjoLib;

std::string ToolsMixed::BinHex2Str(const unsigned char* data, int len) const
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    return ss.str();
}

void ToolsMixed::AnimationPropeller(int * idx) const
{
    const string anim = "|/-\\";
    AnimationCustom(idx, anim);
}

void ToolsMixed::Animation09AZ(int * idx) const
{
    const string anim = "0123456789ABCDEFGHIJKLMNOPQRSTUWXYZ";
    AnimationCustom(idx, anim);
}

void ToolsMixed::AnimationCustom(int * idx, const std::string & animSeries) const
{
    cout << '\b' << animSeries.at((*idx)++ % animSeries.size()) << flush;
}

std::map<std::string, std::string> ToolsMixed::FromPythonDict(const std::string & dictStr) const
{
    std::map<std::string, std::string> ret;
    if (dictStr.empty())
        return ret;
    if (dictStr.front() != '{')
        throw std::invalid_argument("Doesn't start with {\n" + dictStr);
    if (dictStr.back() != '}')
        throw std::invalid_argument("Doesn't start with }\n" + dictStr);

    const std::string & dict = dictStr.substr(1, dictStr.size() - 2);
    const Tokenizer tok;
    const CharManipulations cman;
    for (const auto & token : tok.Tokenize(dict, ','))
    {
        const auto & keyVal = tok.Tokenize(token, ':');
        const string key = cman.Replace(keyVal.at(0), " ", "");;
        const string keyNoQuotes = cman.Replace(key, "'", "");
        const string val = cman.Replace(keyVal.at(1), " ", "");;
        ret[keyNoQuotes] = val;
    }
    return ret;
}

void ToolsMixed::SystemCallWarn(const std::string & command, const std::string & functionName) const
{
    if (int err = system(command.c_str()))
    {
        std::cout << SystemCallPrepareMessage(command, functionName, err) << std::endl;
    }
}

bool ToolsMixed::SystemCallWarnBool(const std::string & command, const std::string & functionName) const
{
    if (int err = system(command.c_str()))
    {
        std::cout << SystemCallPrepareMessage(command, functionName, err) << std::endl;
        return false;
    }
    return true;
}

void ToolsMixed::SystemCallThrow(const std::string & command, const std::string & functionName) const
{
    if (int err = system(command.c_str()))
        throw std::runtime_error(SystemCallPrepareMessage(command, functionName, err));
}

std::string ToolsMixed::SystemCallPrepareMessage(const std::string & command, const std::string & functionName, int err) const
{
    std::ostringstream oss;
    oss << functionName << ":\nCouldn't call command = \n" << command << "\nerr = " << err;
    return oss.str();
}
