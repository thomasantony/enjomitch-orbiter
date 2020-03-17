#ifndef TIME_H
#define TIME_H

#include <string>
#include <ctime>

namespace EnjoLib
{
class Time
{
    public:
        Time();
        virtual ~Time();

        std::string CurrentDateTime() const;
        std::string CurrentTime() const;
        std::string CurrentDateTimeFormated(const std::string & format) const;
        std::string TimeStampToHuman(const time_t rawtime) const;
        std::string TimeStampToHuman(const time_t rawtime, const std::string & format) const;
        bool IsWeekend() const;


    protected:
    private:
};
}
#endif // TIME_H
