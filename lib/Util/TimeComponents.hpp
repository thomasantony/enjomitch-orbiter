#ifndef TIMECOMPONENTS_H
#define TIMECOMPONENTS_H

#include <string>

namespace EnjoLib
{
class TimeComponents
{
    public:
        TimeComponents(double seconds);
        virtual ~TimeComponents();

        std::string ToString() const;

    protected:

    private:
        std::string MakeStr();

        double m_seconds;
        int m_minutes;
        int m_hours;
        std::string m_str;
};
}
#endif // TIMECOMPONENTS_H
