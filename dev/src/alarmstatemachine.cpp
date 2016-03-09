#include "alarmstatemachine.h"
#include "src/alarmoutput.h"

void AlarmStateMachine::UpdateStates(int tEllapsed)
{
    std::vector<IAlarmOutput*>::iterator it;
    for (it=vAlarmOutputs.begin(); it <vAlarmOutputs.end(); it++)
    {

    }
}

AlarmStateMachine::AlarmStateMachine()
{
}

IAlarmOutput* AlarmStateMachine::getAlarmOutput()
{
    IAlarmOutput *result = new AlarmOutput();
    vAlarmOutputs.push_back(result);
    return new AlarmOutput();
}
