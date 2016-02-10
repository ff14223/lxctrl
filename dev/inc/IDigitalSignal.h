#ifndef IDIGITALSIGNAL_H
#define IDIGITALSIGNAL_H

class IDigitalSignal
{
public:
    virtual bool get()=0;
    virtual void set(bool value)=0;
    virtual void updateInput()=0;
    virtual void updateOutput()=0;
    virtual bool raised()=0;
    virtual void setSimulationMode(bool value)=0;
    virtual bool getSimulationMode()=0;
    virtual void setSimulationValue(bool value)=0;

};

#endif // IDIGITALSIGNAL_H
