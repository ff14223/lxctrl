#ifndef ALARM_H
#define ALARM_H

#include "inc/IAlarm.h"

class Alarm : public IAlarm
{
public:
    Alarm();
    virtual void raise();
};

#endif // ALARM_H
