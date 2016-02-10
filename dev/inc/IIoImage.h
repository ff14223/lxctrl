#ifndef IIOIMAGE_H
#define IIOIMAGE_H

#include <string>
#include "inc/IDigitalSignal.h"

class IIoImage
{
public:
    virtual void UpdateInputs()=0;
    virtual void UpdateOutputs()=0;
    virtual IDigitalSignal* getSignal(const std::string SignalName)=0;
    virtual void DumpSignals()=0;
    virtual void KeyPressed(char key)=0;
    virtual int getNrSimulationMappings()=0;
};

#endif // IIOIMAGE_H
