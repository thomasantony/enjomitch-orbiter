#ifndef EnergyPitchProgram_H
#define EnergyPitchProgram_H

#include "Systems/Point.hpp"
#include <vector>
class MFDDataLaunchMFD;

class EnergyPitchProgram
{
    public:
        EnergyPitchProgram();
        virtual ~EnergyPitchProgram();
        double GetPitch( const MFDDataLaunchMFD * data, double time );
        double GetEnergy( const MFDDataLaunchMFD * data ) const;
        bool IsStopCondition( const MFDDataLaunchMFD * data ) const;
    protected:
    private:
        double LinearInterpolationProgramme( const std::vector< EnjoLib::Point > & programme, double x ) const;

        std::vector< EnjoLib::Point > programme;
        bool Flydown;
};

#endif // EnergyPitchProgram_H
