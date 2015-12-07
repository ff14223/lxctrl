#ifndef DIGITALSIGNAL_H
#define DIGITALSIGNAL_H

#include <inc/IDigitalSignal.h>

class DigitalSignal : public IDigitalSignal
{
    bool* m_IoValue;
    bool  m_CurrentValue;
    bool  m_Raised;
public:
    DigitalSignal(bool *value);
    virtual bool get();
    virtual void set(bool value);
    virtual void updateInput();
    virtual void updateOutput();
    virtual bool raised();
};

#endif // DIGITALSIGNAL_H
