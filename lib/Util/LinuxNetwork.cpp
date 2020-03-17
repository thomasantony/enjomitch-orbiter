#include "LinuxNetwork.hpp"
#include <fstream>
#include <stdexcept>

using namespace std;
using namespace EnjoLib;

LinuxNetwork::LinuxNetwork(){}
LinuxNetwork::~LinuxNetwork(){}

std::string LinuxNetwork::GetMacAddress(const char * interface) const
{
    // cat /sys/class/net/eth0/address
    std::string fileName = InterfaceToFilename(interface);
    ifstream addr(fileName);
    if (!addr.is_open())
        throw runtime_error(fileName + " not available!");
    string line;
    getline(addr, line);
    return line;
}

bool LinuxNetwork::HasMacAddress(const char * interface) const
{
    // cat /sys/class/net/eth0/address
    std::string fileName = InterfaceToFilename(interface);
    ifstream addr(fileName);
    return addr.is_open();
}

std::string LinuxNetwork::InterfaceToFilename(const char * interface) const
{
    return string("/sys/class/net/") + interface + "/address";
}
