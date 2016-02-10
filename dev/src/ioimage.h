#ifndef IOIMAGE_H
#define IOIMAGE_H

#include <inc/IDigitalSignal.h>
#include <inc/IIoImage.h>

#include <map>
#include <string>

#include <src/canio.h>
#include <vector>
#include "inc/ISystemData.h"

using namespace std;


typedef enum
{
    NF_IN=0,
}enDigitalIO;

class ioimage : public IIoImage
{
    ISystem *m_pSystem;
    CanIo *m_pCanIo;
    vector<CanNode> vCanIo;


    bool * m_DigitalSignal;     // Signals that get connected internal

    std::map<std::string, IDigitalSignal*> m_mapSignal;
    std::map<std::string, IDigitalSignal*> m_mapActiveSignal;       // interne Signale (cfg intern )
    std::map<std::string, string> m_mapKeyToSignal;

    void MakeSignal(std::string SignalName, std::string SignalMap);
    void GenerateInternalSignals();
public:
    ioimage(ISystem *pSystem);
    IDigitalSignal* getSignal(const std::string SignalName);
    void UpdateInputs();
    void UpdateOutputs();
    void DumpSignals();
    void KeyPressed(char key);
    int getNrSimulationMappings();
};


IIoImage* getIOImage() ;

#endif // IOIMAGE_H
