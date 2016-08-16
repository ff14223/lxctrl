#ifndef IALARM_H
#define IALARM_H

class IAlarm
{
    public:
        virtual void raise()=0;
        virtual void reset()=0;
};
#endif // IALARM_H
