#ifndef ALARM_H
#define ALARM_H

#include "inc/IAlarm.h"
#include "inc/ISystemData.h"
#include "inc/IDigitalSignal.h"

class Alarm : public IAlarm
{
    int m_id;
    IDigitalSignal *m_pSignal;
    ISystemData *m_pSystemData;
public:
    Alarm(const char *Name, ISystemData *pISystem);
    virtual void raise();
};

#endif // ALARM_H
