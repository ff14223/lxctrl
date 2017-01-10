#ifndef CANNODE_H
#define CANNODE_H


#include <map>
#include <string>
#include "inc/IDigitalSignal.h"

#include <linux/can.h>
#include <linux/can/raw.h>

using namespace  std;

class ioimage;
class CanIo;

class CanNode
{
    int m_NodeNr;
    string m_Name;
    bool m_Updated;

    bool m_DigitalInput[64];     // Signals that get connected internal
    bool m_DigitalOutput[64];     // Signals that get connected internal

    int m_state;           // Statemachine
    int m_state_index;     // Statemachine Index

    int m_o_state;           // Statemachine
    int m_i_state;           // Statemachine
    int m_o_state_count;
    canid_t m_CanIdDi, m_CanIdDo, m_CanIdCmdReq, m_CanIdCmdResp;

    void UpdateDigitalInputs();
    void UpdateDigitalOutputs();

    int m_ConfiguredModuleCount;
    unsigned char m_ModuleTypes[8]; // Konfigurierte Module

public:
    CanNode(int NodeNr, string Name);
    int getNodeNumber(){ return m_NodeNr; }
    bool * getDigitalInput(int index){ return &m_DigitalInput[index]; }
    void GenerateSignals(ioimage*image);

    void ReceiveFrame(struct can_frame *frame);

    void getDoFrame(struct canfd_frame   *frame);
    void getCmdFrame(struct canfd_frame   *frame);

    int getState();
    void DumpInfo();


    void InputStatemachine();
    void OuputStatemachine(CanIo *pIO, struct can_frame *frame);
    void SetUpdated(bool value);
    bool GetUpdated();
};

#endif // CANNODE_H
