#include "cannode.h"
#include <iostream>

#include "stdio.h"

#include "src/vds/digitalsignal.h"
#include "src/ioimage.h"


CanNode::CanNode(int NodeNr, string Name)
{
    m_NodeNr = NodeNr;
    m_Name = Name;

    m_CanIdCmdReq = NodeNr + 1565;
    m_CanIdCmdResp = NodeNr + 1629;
    m_CanIdDi = 286 + (NodeNr-1)*4;
    m_CanIdDo = 414 + (NodeNr-1)*4;

    m_o_state = 0;
    m_i_state = 0;

    /* const Setting &s2 = getSettings()->get("CAN.nodes");
    int count = s2.getLength();
    for(int i = 0; i < count; ++i)
    {
          const Setting &signal = s2[i];
          string Name;
          int iNodeNr;
          signal.lookupValue("node_nr", iNodeNr );
          signal.lookupValue("name", Name);

          cout << "    adding can node " << iNodeNr << " as " << Name << endl;
          m_mapNodes[iNodeNr] = new CanNode( iNodeNr, Name );;
    }*/


}

void CanNode::SetUpdated(bool value){m_Updated=value;}
bool CanNode::GetUpdated(){return m_Updated; }

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

unsigned char PackDigitialSignalsInByte( bool *m_DigitalSignal, int start )
{
    unsigned char d=0;

    for(int i=0;i<8;i++)
    {
        if( m_DigitalSignal[i+start] == false)
            continue;
        d |= (1<<i);
    }
    return d;
}

void CanNode::UpdateDigitalInputs()
{
    unsigned char d = PackDigitialSignalsInByte(m_DigitalInput,0);

}

void CanNode::UpdateDigitalOutputs()
{
    unsigned char d = PackDigitialSignalsInByte(m_DigitalOutput,0);

}


void CanNode::getDoFrame(struct canfd_frame *frame)
{

}

void CanNode::getCmdFrame(struct canfd_frame *frame)
{
    frame->can_id = m_CanIdCmdReq;
}

int CanNode::getState()
{
    return m_state;
}

void CanNode::InputStatemachine()
{

}

void CanNode::OuputStatemachine(CanIo *pIO,struct can_frame *frame)
{
    struct canfd_frame oframe;

    switch (m_o_state)
    {
        case 0:     /* request slave status */
            getCmdFrame( &oframe );
            oframe.data[0] = 0;
            oframe.data[1] = 0;
            pIO->Send(&oframe);
            m_o_state = 1;
            m_state_index = 0;
            break;

        case 1:
            if( frame != NULL && frame->can_id == m_CanIdCmdResp)
                m_o_state = 2;
            break;

        case 2:
            getCmdFrame( &oframe );
            oframe.data[0] = m_state_index;
            oframe.data[1] = 0;
            pIO->Send(&oframe);
            if( m_state_index == 7 )
                m_o_state = 10;
            else
                m_o_state = 3;
            break;

        case 3:
            if( frame != NULL && frame->can_id == m_CanIdCmdResp )
            {
                cout << "ModuleType = " << frame->data[0] << " " << frame->data[0] << "\n\r";
                m_o_state = 2;
            }
            break;

        case 10:
            break;


        default:
            break;
    }
}

void CanNode::ReceiveFrame(struct can_frame *frame)
{
    switch( m_state )
    {
    case 0:     /* Wait for CMD Response */
        if( frame && frame->can_id == m_CanIdCmdResp )
            m_state = 100;
        break;

    case 1:
        /* Node is in running state. eg. receiving and sending IO */
    case 100:
        /* extract DI */
        frame->data;
        break;

    case 200:   /* enter error state */
        /* set io Off */
        UpdateDigitalOutputs();
        break;

    default:
        m_state = 0;;
        break;
    }
}


