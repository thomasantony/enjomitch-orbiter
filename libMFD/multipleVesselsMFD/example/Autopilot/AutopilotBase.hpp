#ifndef AUTOPILOTBASE_HPP_INCLUDED
#define AUTOPILOTBASE_HPP_INCLUDED
#include "../MyMFDData.hpp"


struct Point;

class AutopilotBase
{
public:
  AutopilotBase();
  virtual void Guide( MyMFDData * data ) const;
  virtual bool NeedPitch( MyMFDData * data ) const = 0;
  virtual bool NeedYaw( MyMFDData * data ) const = 0;
  virtual bool NeedMECO( MyMFDData * data ) const = 0;

  protected:

  private:
    bool IsLaunched( MyMFDData * data ) const;
};

  #endif // AUTOPILOTBASE_HPP_INCLUDED
