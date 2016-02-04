#ifndef CANIO_H
#define CANIO_H

#include "src/cannode.h"
#include <linux/can.h>
#include <linux/can/raw.h>

#include <vector>

#include "inc/ISystemData.h"

using namespace std;

class CanIo
{
    int m_Socket;
    string m_strDevice;
    std::map<int, CanNode*> m_mapNodes;

    int m_IdCmdReq, m_IdCmdResp, m_IdDi, mIdDo;
    ISystem *m_pSystem;
public:
    CanIo(ISystem*pSystem);
    void GenerateSignals(std::map<std::string, IDigitalSignal*> *map);
    void LoadSettings();
    int Receive(struct can_frame *frame);
    int Send(struct can_frame *frame);
    void Input();
    void Output();
    void DumpInfo();
};

#endif // CANIO_H
