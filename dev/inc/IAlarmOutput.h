#ifndef IALARMOUTPUT_H
#define IALARMOUTPUT_H

class IAlarmOutput
{
public:
    virtual void StateMachine(int tEllapsed)=0;
    virtual void Raise()=0;
    virtual void Reset()=0;
};


#endif // IALARMOUTPUT_H

