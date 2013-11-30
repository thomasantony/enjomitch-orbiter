#include "EnergyPitchProgram.hpp"
#include "MFDDataLaunchMFD.hpp"
#include "Math/GeneralMath.hpp"
#include "Utils/VesselCapabilities.hpp"

using namespace EnjoLib;

EnergyPitchProgram::EnergyPitchProgram()
{
    // Energy - Pitch
    programme.push_back( Point(0.0, 89.0) );
    programme.push_back( Point(0.4, 80.0) );
    programme.push_back( Point(0.5, 70.0) );
    programme.push_back( Point(0.8, 55.0) );
    programme.push_back( Point(1.2, 48.0) );
    programme.push_back( Point(1.7, 43.0) );
    programme.push_back( Point(2.8, 35.0) );
    programme.push_back( Point(4.0, 24.0) );
    programme.push_back( Point(7.0, 18.0) );
    programme.push_back( Point(10.0, 12.0) );
    programme.push_back( Point(16.0, 7.0) );
    programme.push_back( Point(22.0, 5.0) );
    programme.push_back( Point(28.0, 2.5) );
    programme.push_back( Point(40.0, 0) );

    Flydown = false;
}

EnergyPitchProgram::~EnergyPitchProgram()
{
    //dtor
}

double EnergyPitchProgram::LinearInterpolationProgramme( const std::vector< Point > & programme, double x ) const
{
    if ( programme.empty() )
        return 0;

    if ( x < programme.front().x )
        return programme.front().y;

    if ( x > programme.back().x )
        return programme.back().y;

    for (size_t i = 1; i < programme.size(); ++i)
    {
        if ( programme[i].x < x )
            continue;
        else
            return GeneralMath().LinearInterpol(x, programme[i-1], programme[i]);
    }

    return 0;
}

double EnergyPitchProgram::GetEnergy( const MFDDataLaunchMFD * data ) const
{
    const double PitchMult = 1;
    const VECTOR3 & vel = data->GetMovParams().m_vel;
    double velMod = data->GetMovParams().m_velMod;
    double energy = data->GetCurrentAlt() + velMod*velMod;
    energy *= 1e-6;
    energy *= PitchMult;

    return energy;
}

double EnergyPitchProgram::GetPitch( const MFDDataLaunchMFD * data, double time )
{
    VESSEL * v = data->GetVessel();
    const double r = data->m_bodyPhys.radius + data->GetCurrentAlt();
    const double gravitAccel = data->GetMi() / ( r * r );

    double energy = GetEnergy( data );
    double Pitch_SP = LinearInterpolationProgramme(programme, energy );

    //  sprintf( oapiDebugString(), "energy = %.02lf, pitch = %.02lf", energy, Pitch_SP);

    ELEMENTS elements;
    v->GetElements(NULL, elements, NULL, 0,FRAME_EQU);
    double Alt_Apoapsis = (elements.a * ( 1 + elements.e )/1000) - 6371;
    /*		if (Alt_Apoapsis > 170.0) {
    			Pitch_SP = Pitch_SP/4.0;			// Flydown -- we've met our altitude goal
    			if ((GetAOA()*DEG < 2.0))
    				Pitch_SP += 1-1.0*GetAOA()*DEG;
    		}
    */
    double EngineThrust = VesselCapabilities().GetMainThrustParm(v).F;
    const double ScreenHeight = 125.0;
    if (Alt_Apoapsis < 1.5*ScreenHeight)
        if (v->GetThrusterCount() < 22)			// Special low-power routine
            if (EngineThrust/gravitAccel < 0.6*v->GetMass())
            {
                Pitch_SP += 8.0*((0.6*v->GetMass()) / (EngineThrust/gravitAccel));
                if (Pitch_SP > 40.0) Pitch_SP = 40.0;
            }
    VECTOR3 horiz;
    v->GetHorizonAirspeedVector(horiz);

    if ((Alt_Apoapsis > ScreenHeight) || (horiz.y < -10.0) || (Flydown))
    {
        Flydown = true;

//			Pitch_SP *= 0.25;					// Flydown -- we've met our altitude goal
        double Pitch_zero = -0.012*horiz.y;
        double Aggressive = 0.98-(sqrt(energy)*0.0156);
//			double Aggressive = 0.85+(energy*0.002);
        if (Aggressive > 0.98) Aggressive = 0.98;
        if (horiz.y > 0.0)
        {
            Aggressive = 0.98-(horiz.y/1000);
            if (Aggressive < 0.85)
                Aggressive = 0.85;
        }
       // double RollAtt = 0;
      //  if (RollAtt == 0)
      //      Pitch_SP += Pitch_zero-Aggressive*v->GetAOA()*DEG;
      //  else if (RollAtt == 1)
            Pitch_SP += Pitch_zero+Aggressive*v->GetAOA()*DEG;
        if (Alt_Apoapsis < 1.5*ScreenHeight)
            if (v->GetThrusterCount() < 22)						// Special low-power addition
                if (EngineThrust/gravitAccel < 0.6*v->GetMass())
                {
                    double multiple = 8.0;
                    multiple /= (horiz.y/25);
                    Pitch_SP += multiple*((0.6*v->GetMass()) / (EngineThrust/gravitAccel));
                }
        if (Pitch_SP > 40.0) Pitch_SP = 40.0;
    }
    else Pitch_SP += -0.02*horiz.y;
//		else if ((Alt_Apoapsis > 120.0) && (GetAOA()*DEG < 2.0))
//			Pitch_SP += 1-1.0*GetAOA()*DEG;


    /*		if (Alt_Apoapsis < ScreenHeight)
    			if (Pitch_SP < (ScreenHeight - Alt_Apoapsis)/5) {
    				Flydown = true;
    				Pitch_SP = (ScreenHeight - Alt_Apoapsis)/5;
    			}
    */
    double MinPitch = 0.0001; // not 0, because 0 is a special case
    if (Pitch_SP < MinPitch)
    {
        Pitch_SP = MinPitch;
        Flydown = true;
    }

    double pitchRad = Pitch_SP * RAD;
    return pitchRad;
}


bool EnergyPitchProgram::IsStopCondition( const MFDDataLaunchMFD * data ) const
{
    double r = data->m_bodyPhys.radius;
    double mi = data->GetMi(); //( m^3/sec^2)

    double alt = data->GetCurrentAlt();
    double a = ((data->ApA + r) + (alt + r)) / 2;
    double e = ((data->ApA + r) / a ) - 1;
    double h = sqrt(mi * a * ( 1 - (e * e)));
    double velPeriapsis = h / (alt + r);

    // Get current velocity
    double velMod = data->GetMovParams().m_velMod;
    double energy = GetEnergy( data );
    if ((energy > 10.0) && (velMod > velPeriapsis))
    {
        return true;
    }
    return false;
}

