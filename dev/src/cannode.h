#ifndef CANNODE_H
#define CANNODE_H


#include <map>
#include <string>
#include "inc/IDigitalSignal.h"
#include "stdint.h"


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

    unsigned char io_image_in[8];   //
    unsigned char io_image_out[8];

    bool m_DigitalInput[64];     // Signals that get connected internal
    bool m_DigitalOutput[64];     // Signals that get connected internal

    int m_state;           // Statemachine
    int m_state_index;     // Statemachine Index

    volatile int m_o_state;           // Statemachine
    volatile int m_i_state;           // Statemachine
    int m_o_state_count;
    canid_t m_CanIdDi, m_CanIdDo, m_CanIdCmdReq, m_CanIdCmdResp;
    canid_t m_CanIdAlarm;


    int m_ConfiguredModuleCount;
    unsigned char m_ModuleTypes[8]; // Konfigurierte Module

    /* private functions */

    void CopySignalsToImage(uint64_t *p);
    void CopySignalsFromImage(uint64_t *p);



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

    void Statemachine(CanIo *pIO,struct can_frame *frame);
    void InputStatemachine();
    void OuputStatemachine(CanIo *pIO, struct can_frame *frame);

    void UpdateDigitalInputs();
    void UpdateDigitalOutputs(CanIo *pIO);

    void SetUpdated(bool value);
    bool GetUpdated();
};

#endif // CANNODE_H
