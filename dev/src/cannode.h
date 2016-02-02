#ifndef CANNODE_H
#define CANNODE_H


#include <map>
#include <string>
#include "inc/IDigitalSignal.h"

using namespace  std;


class CanNode
{
    int m_NodeNr;
    string m_Name;
    bool m_DigitalInput[64];     // Signals that get connected internal
    bool m_DigitalOutput[64];     // Signals that get connected internal

public:
    CanNode(int NodeNr, string Name);
    int getNodeNumber(){ return m_NodeNr; }
    bool * getDigitalInput(int index){ return &m_DigitalInput[index]; }
    void GenerateSignals(std::map<std::string, IDigitalSignal*> *map);
};

#endif // CANNODE_H
