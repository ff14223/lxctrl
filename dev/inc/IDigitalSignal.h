#ifndef IDIGITALSIGNAL_H
#define IDIGITALSIGNAL_H

class IDigitalSignal
{
public:
    virtual bool get()=0;
    virtual void set(bool value)=0;
};

#endif // IDIGITALSIGNAL_H
