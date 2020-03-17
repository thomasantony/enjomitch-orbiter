#ifndef LINUXTOOLS_H
#define LINUXTOOLS_H

#include <string>

namespace EnjoLib
{
// echo -n `cat /sys/class/net/eth0/address` | openssl sha256
class LinuxTools
{
    public:
        LinuxTools();
        virtual ~LinuxTools();
        std::string HashSHA256(const std::string & data) const;

    protected:

    private:
};
}

#endif // LINUXTOOLS_H
