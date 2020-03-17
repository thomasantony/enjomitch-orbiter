#ifndef IFSTREAM_H
#define IFSTREAM_H

#include <fstream>
#include <string>

namespace EnjoLib
{
class Ifstream : public std::ifstream
{
    public:
        Ifstream(const char * fileName, bool tryOpen = true);
        Ifstream(const std::string & fileName, bool tryOpen = true);
        virtual ~Ifstream();

        const std::string & GetFileName() const { return m_fileName; }
        void IsOpenThrow() const;

    protected:

    private:
        const std::string m_fileName;
};
}

#endif // IFSTREAM_H
