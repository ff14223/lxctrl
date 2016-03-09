#ifndef ALARMSTATEMACHINE_H
#define ALARMSTATEMACHINE_H

#include "inc/IAlarmOutput.h"
#include <vector>

class AlarmStateMachine
{
    std::vector<IAlarmOutput*> vAlarmOutputs;
public:
    AlarmStateMachine();
    void UpdateStates(int tEllapsed);
    IAlarmOutput* getAlarmOutput();
};

#endif // ALARMSTATEMACHINE_H
