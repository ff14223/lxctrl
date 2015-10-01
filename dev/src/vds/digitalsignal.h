#ifndef DIGITALSIGNAL_H
#define DIGITALSIGNAL_H

#include <inc/IDigitalSignal.h>

class DigitalSignal : public IDigitalSignal
{
    bool* m_value;
public:
    DigitalSignal(bool *value);
    virtual bool get();
    virtual void set(bool value);
};

#endif // DIGITALSIGNAL_H
