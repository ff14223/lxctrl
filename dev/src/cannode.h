#ifndef CANNODE_H
#define CANNODE_H


#include <map>
#include <string>
#include "inc/IDigitalSignal.h"

#include <linux/can.h>
#include <linux/can/raw.h>


using namespace  std;


class CanNode
{
    int m_NodeNr;
    string m_Name;
    bool m_DigitalInput[64];     // Signals that get connected internal
    bool m_DigitalOutput[64];     // Signals that get connected internal
    int m_state;
    int m_CanIdDi, m_CanIdDo, m_CanIdCmdReq, m_CanIdCmdResp;
    unsigned char PackDigitialInputsByte( int start );
public:
    CanNode(int NodeNr, string Name);
    int getNodeNumber(){ return m_NodeNr; }
    bool * getDigitalInput(int index){ return &m_DigitalInput[index]; }
    void GenerateSignals(std::map<std::string, IDigitalSignal*> *map);
    void StateMachine(struct can_frame *frame);

    void getDoFrame(struct can_frame *frame);
    int getState();
    void DumpInfo();
};

#endif // CANNODE_H
