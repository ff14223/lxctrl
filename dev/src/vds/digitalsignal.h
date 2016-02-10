#ifndef DIGITALSIGNAL_H
#define DIGITALSIGNAL_H

#include <inc/IDigitalSignal.h>

class DigitalSignal : public IDigitalSignal
{
    bool* m_IoValue;
    bool  m_CurrentValue;
    bool  m_Raised;
    bool  m_Simulate;
    bool  m_SimulationValue;
public:
    DigitalSignal(bool *value);
    virtual bool get();
    virtual void set(bool value);
    virtual void updateInput();
    virtual void updateOutput();
    virtual bool raised();
    virtual void setSimulationMode(bool value);
    virtual bool getSimulationMode();
    virtual void setSimulationValue(bool value);
};

#endif // DIGITALSIGNAL_H
