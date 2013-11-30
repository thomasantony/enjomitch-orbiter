#ifndef AUTOPILOTMANAGER_HPP_INCLUDED
#define AUTOPILOTMANAGER_HPP_INCLUDED
#include "AutopilotType.hpp"

class AutopilotBase;
class AutopilotStandard;
class AutopilotSophisticated;

class AutopilotManager
{

public:

  AutopilotBase * GetAP(AutopilotType type);

  AutopilotManager();
  ~AutopilotManager();
private:


  AutopilotStandard * pAPStandard;
  AutopilotSophisticated * pAPSophisticated;
};

#endif // AUTOPILOTMANAGER_HPP_INCLUDED
