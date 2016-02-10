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

    /* mapping von io <-> Signal und SignalName auf Signal */
    std::map<std::string, IDigitalSignal*> m_mapSignal;

    /* liste mit akiven Signalen */
    std::map<std::string, IDigitalSignal*> m_mapActiveSignal;       // interne Signale (cfg intern )
    std::map<std::string, string> m_mapKeyToSignal;
    std::map<std::string, string> m_mapMappingSignal;

    void MakeSignal(std::string SignalName, std::string SignalMap);
    void GenerateInternalSignals();
public:
    void AddSignal(string SignalName, IDigitalSignal *pSignal);
    ioimage(ISystem *pSystem);
    IDigitalSignal* getSignal(const std::string SignalName);
    void UpdateInputs();
    void UpdateOutputs();
    void DumpSignals();
    void KeyPressed(char key);
    int getNrSimulationMappings();
    void CheckSignals();
    void GenerateSignals();
};


IIoImage* getIOImage() ;

#endif // IOIMAGE_H
