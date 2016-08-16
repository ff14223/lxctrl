#ifndef ALARMSTATEMACHINE_H
#define ALARMSTATEMACHINE_H

#include "inc/IAlarmOutput.h"
#include "inc/IAlarm.h"
#include "inc/ialarmstatemaschine.h"

#include <map>
#include "inc/ISystemData.h"

class AlarmStateMachine : public IAlarmStateMachine
{
    ISystem *pSystem;
    std::map<std::string, IAlarmOutput*> m_mapAlarmOutput; 
    std::map<std::string, IAlarm*> m_mapAlarm;
    
public:
    AlarmStateMachine(ISystem *pSystem);
    void UpdateStates(int tEllapsed);
    IAlarmOutput* getAlarmOutput(std::string name);
    IAlarm* getAlarm(string name);
    IAlarm* loadAlarm(string name);
    void LoadConfiguration();
};

#endif // ALARMSTATEMACHINE_H
