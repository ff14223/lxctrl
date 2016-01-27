#ifndef IOIMAGE_H
#define IOIMAGE_H

#include <inc/IDigitalSignal.h>
#include <inc/IIoImage.h>

#include <map>
#include <string>

typedef enum
{
    NF_IN=0,
}enDigitalIO;

class ioimage : public IIoImage
{
    bool * m_DigitalSignal;     // Signals that get connected internal

    std::map<std::string, IDigitalSignal*> m_mapSignal;
    std::map<std::string, IDigitalSignal*> m_mapInternSignal;       // interne Signale (cfg intern )
    std::map<std::string, IDigitalSignal*> m_mapCanSignal;          // interne Signale (cfg intern )
    std::map<std::string, IDigitalSignal*> m_mapLokalSignal;       // interne Signale (cfg intern )
    void MakeSignal(std::string SignalName, std::string SignalMap);
    void GenerateInternalSignals();
public:
    ioimage();
    IDigitalSignal* getSignal(const std::string SignalName);
    void UpdateInputs();
    void UpdateOutputs();
    void DumpSignals();
};


IIoImage* getIOImage() ;

#endif // IOIMAGE_H
