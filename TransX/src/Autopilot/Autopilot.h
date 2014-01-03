#ifndef AUTOPILOT_H
#define AUTOPILOT_H


class Autopilot
{
    public:
        Autopilot();
        virtual ~Autopilot();
        void Update(double SimDT);
        void Enable(bool val);
        void SwitchEnabled();
        bool IsEnabled();
    protected:
    private:
        bool m_isEnabled;
};

#endif // AUTOPILOT_H
