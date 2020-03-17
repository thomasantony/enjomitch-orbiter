#ifndef COUTBUF_H
#define COUTBUF_H

#include <iostream>
//#include <iosfwd>

namespace EnjoLib
{
class Log
{
    public:
        Log();
        virtual ~Log(){}

        std::ostream & GetLog(bool verbose);

    private:
        std::ostream m_cnull;
        std::wostream m_wcnull;
};
}


extern std::ostream cnull;
extern std::wostream wcnull;

std::ostream & GetLog(bool verbose);

#endif // COUTBUF_H
