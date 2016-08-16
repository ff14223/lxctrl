#ifndef IALARMSTATEMASCHINE
#define IALARMSTATEMASCHINE

#include "inc/IAlarm.h"
#include "inc/IAlarmOutput.h"
#include <string>

class IAlarmStateMachine
{
public:
    virtual IAlarm* getAlarm(std::string name)=0;
    virtual IAlarm* loadAlarm(std::string name)=0;
    virtual IAlarmOutput* getAlarmOutput(std::string name)=0;
    virtual void UpdateStates(int tEllapsed)=0;
};

#endif // IALARMSTATEMASCHINE

