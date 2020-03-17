#ifndef LINUXNETWORK_H
#define LINUXNETWORK_H

#include <string>

namespace EnjoLib
{
class LinuxNetwork
{
    public:
        LinuxNetwork();
        virtual ~LinuxNetwork();
        std::string GetMacAddress(const char * interface) const;
        bool HasMacAddress(const char * interface) const;

    protected:

    private:
        std::string InterfaceToFilename(const char * interface) const;
};
}


#endif // LINUXNETWORK_H
