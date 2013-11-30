#include "SoundMapMyMFD.hpp"
#include "SoundSampleIDEnum.hpp"

SoundMapMyMFD::SoundMapMyMFD()
{
    this->operator[]( HALF_ORBITAL_VEL ) = "Sound/LaunchMFDEnjo/HalfOV.wav";
    this->operator[]( TARGET_MANUAL ) = "Sound/LaunchMFDEnjo/ManualTarget.wav";
    this->operator[]( TARGET_SELECTED ) = "Sound/LaunchMFDEnjo/TargetSelected.wav";
    this->operator[]( CUT_ENGINES ) = "Sound/LaunchMFDEnjo/CutEngines.wav";
    this->operator[]( ALTITUDE_SET ) = "Sound/LaunchMFDEnjo/alt_set.wav";
    this->operator[]( ALTITUDE_AUTO ) = "Sound/LaunchMFDEnjo/AltitudeAutomatic.wav";
    this->operator[]( HUD_ENABLED ) = "Sound/LaunchMFDEnjo/hud_en.wav";
    this->operator[]( HUD_DISABLED ) = "Sound/LaunchMFDEnjo/hud_dis.wav";
    this->operator[]( BEEP_ENABLED ) = "Sound/LaunchMFDEnjo/beep_en.wav";
    this->operator[]( VOICE_ENABLED ) = "Sound/LaunchMFDEnjo/voice_en.wav";
    this->operator[]( VOICE_DISABLED ) = "Sound/LaunchMFDEnjo/voice_dis.wav";
    this->operator[]( BEEP ) = "Sound/LaunchMFDEnjo/beep_am_light22.wav";
}
