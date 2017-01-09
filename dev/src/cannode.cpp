#include "cannode.h"
#include <iostream>

#include "stdio.h"

#include "src/vds/digitalsignal.h"
#include "src/ioimage.h"
#include "settings.h"

class CanBusExceptionType : public exception
{
    string s1;
    virtual const char* what() const throw()
    {
        // s1.append("Einstellungen konnten nicht geladen werden");
        return s1.c_str();
    }
public:
    void setReason(string reason){s1=reason;}
    virtual ~CanBusExceptionType() throw() { }
} CanBusException;

unsigned char getModuleType(string ModuleName)
{
    if( ModuleName.compare("DO721") == 0 ) return 0x08;
    if( ModuleName.compare("DI439") == 0 ) return 0x0D;
    return 0x00;
}

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

    const Setting &s2 = getSettings()->get((Name + ".modules").c_str());

    int m_ConfiguredModuleCount = s2.getLength();
    for(int i = 0; i < m_ConfiguredModuleCount; ++i)
    {
          const Setting &signal = s2[i];
          string Type;

          signal.lookupValue("type", Type );
          unsigned char typecode = getModuleType(Type);

          if( typecode == 0x00 )
          {
              CanBusException.setReason("Unbekannter Modultype in CAN Konfiguration");
              throw CanBusException;
          }


          cout << "    adding module " << Type << " at position " << i << endl;
          m_ModuleTypes[i] = typecode;
    }


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
    char Text[80];
    unsigned char p1,p2,d1,d2;

    switch (m_o_state)
    {
        case 0:     /* request slave status */
            getCmdFrame( &oframe );
            oframe.data[0] = 0;
            oframe.data[1] = 0;
            pIO->Send(&oframe);
            m_o_state = 1;
            m_state_index = 1;
            break;

        case 1:
            if( frame != NULL && frame->can_id == m_CanIdCmdResp)
                m_o_state = 2;
            break;

        case 2:
            oframe.data[0] = 2;
            oframe.data[1] = 0;
            oframe.data[2] = 0;
            pIO->Send(&oframe);
            m_o_state = 3;
            break;

        case 3:
            if( frame != NULL && frame->can_id == m_CanIdCmdResp )
            {
                // Anzahl der Module in
                if( (m_ConfiguredModuleCount+1) != frame->data[4] )
                {
                    cout << "    Modulanzahl stimmt nicht." << "\r\n";
                    m_o_state = 0;
                }
                else
                {
                    m_o_state = 4;
                }
            }
            break;


        case 4:
            if( m_state_index == m_ConfiguredModuleCount )
                m_o_state = 10;
            else
                m_o_state = 3;

            getCmdFrame( &oframe );

            oframe.data[0] = 0;
            oframe.data[1] = 1;
            oframe.data[2] = m_state_index;
            pIO->Send(&oframe);
            m_state_index++;
            break;

        case 5:
            if( frame != NULL && frame->can_id == m_CanIdCmdResp )
            {
                sprintf(Text, "  Fehler: %02X %02X %02X %02X", frame->data[4], frame->data[5], frame->data[6], frame->data[7] );
            }
            cout << Text << "\r\n";
            m_o_state = 4;
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


