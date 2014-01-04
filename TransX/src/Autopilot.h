#ifndef AUTOPILOT_H
#define AUTOPILOT_H

#include <Systems/Vect3.hpp>
#include <PID/PID.hpp>

class Autopilot
{
    public:
        Autopilot();
        virtual ~Autopilot();
        void Update(double SimDT);
        void Enable(bool val);
        void SwitchEnabled();
        bool IsEnabled();
        VECTOR3 GetVesselAngularAccelerationRatio( const VESSEL * vessel );
    protected:
    private:
        void OnDisabled();
        EnjoLib::PID m_pidAPSpaceX;
        EnjoLib::PID m_pidAPSpaceY;
        EnjoLib::PID m_pidAPSpaceBank;

        bool m_isEnabled;
        const static EnjoLib::Vect3 m_statDeltaGliderRefRotAcc; // reference pitch, yaw and bank angular accelerations
};

#endif // AUTOPILOT_H
