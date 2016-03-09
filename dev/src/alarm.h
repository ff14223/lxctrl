#ifndef ALARM_H
#define ALARM_H

#include "inc/IAlarm.h"
#include "inc/ISystemData.h"
#include "inc/IDigitalSignal.h"
#include "inc/IAlarmOutput.h"

#include <vector>

class Alarm : public IAlarm
{
    int m_id;
    IDigitalSignal *m_pSignal;
    ISystemData *m_pSystemData;
    vector<IAlarmOutput*> vAlarmOutputs;
public:
    Alarm(const char *Name, ISystemData *pISystem);
    void AddOutput(IAlarmOutput* out);
    virtual void raise();
};

#endif // ALARM_H
