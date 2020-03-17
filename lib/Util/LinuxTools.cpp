#include "LinuxTools.hpp"
#include "ToolsMixed.hpp"
#include <openssl/sha.h>
#include <iostream>

using namespace std;
using namespace EnjoLib;

LinuxTools::LinuxTools(){}
LinuxTools::~LinuxTools(){}

std::string LinuxTools::HashSHA256(const std::string & data) const
{
    const int digestLen = SHA256_DIGEST_LENGTH;
    unsigned char hash[digestLen];
    unsigned char input[data.size()];
    for (unsigned i = 0; i < data.size(); ++i)
        input[i] = data[i];
    SHA256(input, data.size(), hash);

    string ret = ToolsMixed().BinHex2Str(hash, digestLen);
    return ret;
}



