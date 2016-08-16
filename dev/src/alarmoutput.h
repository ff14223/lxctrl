#ifndef ALARMOUTPUT_H
#define ALARMOUTPUT_H

#include "inc/IAlarmOutput.h"
#include "inc/IDigitalSignal.h"

class AlarmOutput : public IAlarmOutput
{
    int iState;
    int t;
    int tOn;
    int tDelayOn;
    IDigitalSignal *pSignal;

public:
    AlarmOutput(int tOn, int tDelayOn, IDigitalSignal *pSignal);
    void setSignalRaiseAlarm(IDigitalSignal *pSignal);

    virtual void StateMachine(int tEllapsed);
    virtual void Raise();
    virtual void Reset();
};

#endif // ALARMOUTPUT_H
