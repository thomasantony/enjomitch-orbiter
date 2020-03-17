#ifndef CSTR_H
#define CSTR_H

#include <string>

template <class T>
static std::string Str(const T & t, char sep = '|')
{
    return std::to_string(t) + sep;
}

class CStr
{
    public:
        CStr(char sep = '|');

        template <class T>
        std::string operator() (const T & t) const
        {
            return Str(t, m_sep);
        }

    private:
        char m_sep = 0;

};

#endif // CSTR_H
