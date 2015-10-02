#ifndef IIOIMAGE_H
#define IIOIMAGE_H

#include <string>
#include "inc/IDigitalSignal.h"

class IIoImage
{
public:
    virtual IDigitalSignal* getSignal(const std::string SignalName)=0;
};

#endif // IIOIMAGE_H
