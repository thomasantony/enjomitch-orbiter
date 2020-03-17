#ifndef OFSTREAM_H
#define OFSTREAM_H

#include <fstream>
#include <string>

namespace EnjoLib
{
class Ofstream : public std::ofstream
{
    public:
        Ofstream(const char * fileName, bool tryOpen = true);
        Ofstream(const std::string & fileName, bool tryOpen = true);
        virtual ~Ofstream();

        const std::string & GetFileName() const { return m_fileName; }
        void IsOpenThrow() const;

    protected:

    private:
        const std::string m_fileName;
};
}
#endif // OFSTREAM_H
