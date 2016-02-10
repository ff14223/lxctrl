#include "cannode.h"
#include <iostream>

#include "stdio.h"

#include "src/vds/digitalsignal.h"
#include "src/ioimage.h"

void CanNode::DumpInfo()
{
    cout << "Node " << m_NodeNr << " State:" << m_state << "\r\n";
}

void CanNode::GenerateSignals(ioimage*image)
{
    char text[100];


    for(int i = 0; i < 64; i++ )
    {
        DigitalSignal * d = new DigitalSignal( &m_DigitalInput[i] );

        sprintf(text,"%s.di[%d]",m_Name.c_str(),i);
        std::string SignalName = text;
        image->AddSignal(SignalName, d);
    }

    for(int i = 0; i < 64; i++ )
    {
        DigitalSignal * d = new DigitalSignal( &m_DigitalOutput[i] );

        sprintf(text,"%s.do[%d]",m_Name.c_str(),i);
        std::string SignalName = text;
        image->AddSignal(SignalName, d);
    }
}

unsigned char CanNode::PackDigitialInputsByte( int start )
{
    unsigned char d=0;

    for(int i=0;i<8;i++)
    {
        if( m_DigitalInput[i+start] == false)
            continue;
        d |= (1<<i);
    }
    return d;
}

void CanNode::getDoFrame(struct canfd_frame *frame)
{
    frame->can_id = m_CanIdDo;
    for(int i=0;i<8;i++)
        frame->data[i] = PackDigitialInputsByte(i*8);
}

void CanNode::getCmdFrame(struct canfd_frame *frame)
{
    frame->can_id = m_CanIdCmdReq;
}

int CanNode::getState()
{
    return m_state;
}

void CanNode::StateMachine(struct can_frame *frame)
{
    switch( m_state )
    {
    case 0:     /* Wait for CMD Response */
        if( frame && frame->can_id == m_CanIdCmdResp )
            m_state = 100;
        break;

    case 100:
        /* extract DI */
        break;

    default:
        m_state = 0;;
        break;
    }
}

CanNode::CanNode(int NodeNr, string Name)
{
    m_NodeNr = NodeNr;
    m_Name = Name;

    m_CanIdCmdReq = NodeNr + 1565;
    m_CanIdCmdResp = NodeNr + 1629;
    m_CanIdDi = 286 + (NodeNr-1)*4;
    m_CanIdDo = 414 + (NodeNr-1)*4;
}

