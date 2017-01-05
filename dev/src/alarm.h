#ifndef ALARM_H
#define ALARM_H

#include "inc/IAlarm.h"
#include "inc/ISystemData.h"
#include "inc/IDigitalSignal.h"
#include "inc/IAlarmOutput.h"
#include "inc/iscript.h"

#include <vector>
#include <string>


typedef struct
{
    int tdelayOn;
    IAlarmOutput* pIAlarmOutput;
}AlarmOutputItem;

class Alarm : public IAlarm
{
    int m_id;
    std::string m_text;
    IScript *pScript;
    IDigitalSignal *m_pSignal;
    IDigitalSignal *m_pRaise;
    ISystemData *m_pSystemData;
    vector<IAlarmOutput*> vAlarmOutputs;
public:
    Alarm(std::string Name, ISystemData *pISystem);
    void AddOutput(int tDelayOn, IAlarmOutput* out);
    virtual void raise();
    virtual void reset();
};

#endif // ALARM_H
