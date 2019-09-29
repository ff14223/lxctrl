#include "cannode.h"
#include <iostream>

#include "stdio.h"

#include "src/vds/digitalsignal.h"
#include "src/ioimage.h"
#include "settings.h"
#include "stdint.h"


#define CAN_CMD_GETSTATUS       0x00
#define CAN_CMD_GET_MODULE_TYPE 0x01
#define INFO(x) x;

void BuildCmdCanFrame(unsigned char * data, unsigned char id,unsigned char cmd,unsigned char p1,unsigned char p2)
{
    data[0] = id; data[1] = cmd; data[2] = p1; data[3] = p2;
}

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
    m_CanIdAlarm = 222 + (NodeNr-1);
    m_o_state = 0;
    m_i_state = 0;

    const Setting &s2 = getSettings()->get((Name + ".modules").c_str());

    m_ConfiguredModuleCount = s2.getLength();
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
    cout << "Node " << m_NodeNr << " State in:" << m_i_state << "out:" << m_o_state <<"\r\n";

    cout << "Inputs 0-8:";
    for(int i =0;i<8;i++)
        cout << " " << m_DigitalInput[i];
    cout << "\r\n";

    cout << "Outputs 0-8:";
    for(int i =0;i<8;i++)
        cout << " " << m_DigitalOutput[i];
    cout << "\r\n";

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


void CanNode::CopySignalsToImage(uint64_t *p)
{
    uint64_t d=0;

    for(int i=0;i<64;i++)
    {
        if( m_DigitalInput[i] == false)
            continue;

        cout << "do[" << i << "]=1";
        d |= (1<<i);
    }

    *p = d;
}


void CanNode::CopySignalsFromImage(uint64_t *p)
{

    for(int i=0;i<64;i++)
    {
        if( (*p & (1<<i)) != 0 )
            m_DigitalInput[i] = true;
        else
            m_DigitalInput[i] = false;
    }
}


void CanNode::UpdateDigitalInputs()
{
}

void CanNode::UpdateDigitalOutputs(CanIo *pIO)
{
    struct canfd_frame oframe;

    // only possible in o_state 10
    if( m_o_state != 10 )
        return;

    cout << "do-" << "\r\n";
    getDoFrame( &oframe );

    for(int i=0;i<8;i++)
    {
        unsigned char d = 0;
        for(int k=0;k<8;k++)
            if( m_DigitalOutput[i*8+k] == true )
               d |= (1<<k);
	oframe.data[i]=d; 
    }
    pIO->Send(&oframe);
}

void CanNode::getDoFrame(struct canfd_frame *frame)
{
    frame->can_id = m_CanIdDo;
}

void CanNode::getCmdFrame(struct canfd_frame *frame)
{
    frame->can_id = m_CanIdCmdReq;
}

int CanNode::getState()
{
    return m_state;
}


void CanNode::Statemachine(CanIo *pIO,struct can_frame *frame)
{
    struct canfd_frame oframe;
    char Text[80];
    unsigned char p1,p2,d1,d2;


    if( frame != NULL)
    {
        // Fehler sind Frames mit Bit 7 = 1 oder 0xFF in der Kennung
        if( frame->data[0] == 0xFF || (frame->data[0] & 0x80) == 0x80 )
            m_o_state = 20;
        if( frame->can_id == m_CanIdAlarm )
	{
	  cout << "alarm:" << "\r\n";
	  for(int i=0;i<8;i++)
	     cout << " " << (int)frame->data[i];
          cout << "\r\n";
	}
    }

    switch (m_o_state)
    {
        case 0:     /* request slave status */
            INFO(cout << "Request Slave Status\r\n")
            getCmdFrame(&oframe);
            BuildCmdCanFrame(oframe.data, 0,CAN_CMD_GETSTATUS, 0, 0);
            pIO->Send(&oframe);
            m_o_state_count=0;
            m_o_state = 1;
            break;

        case 1:
            if( m_o_state_count++ > 100 )   // timeout
                m_o_state = 0;

            if( frame != NULL && frame->can_id == m_CanIdCmdResp)
                m_o_state = 2;
            break;

        case 2:   // get module count p1 == 0
            getCmdFrame( &oframe);
            BuildCmdCanFrame( oframe.data, 0, CAN_CMD_GET_MODULE_TYPE , 0, 0);
            pIO->Send(&oframe);
            m_o_state_count = 0;
            m_o_state = 3;
            break;

        case 3:
            m_state_index = 0;

            if( m_o_state_count++ > 100 )
                m_o_state = 0;

            if( frame != NULL && frame->can_id == m_CanIdCmdResp )
            {
                // Anzahl der Module in
                if( m_ConfiguredModuleCount != frame->data[4])
                {
                    cout << "    Modulanzahl stimmt nicht." << "\r\n";
		    cout << "Reported:" << (int) frame->data[4] << 
			" configured:" << m_ConfiguredModuleCount <<  "\r\n";
                    m_o_state = 20;
                }
                else
                {
                    m_o_state = 4;
                }
            }
            break;


        case 4:
            if( m_state_index >= m_ConfiguredModuleCount )
	    {
		cout << "Enter Run State\r\n";
                m_o_state = 10;
	    }
            else
            {
                m_o_state = 5;

                getCmdFrame( &oframe );
                BuildCmdCanFrame( oframe.data, m_state_index, CAN_CMD_GET_MODULE_TYPE , m_state_index+1, 0);
                pIO->Send(&oframe);
                m_state_index++;
            }
            m_o_state_count = 0;
            break;

        case 5:
            if( m_o_state_count++ > 100 )
                m_o_state = 0;

            if( frame != NULL && frame->can_id == m_CanIdCmdResp )
            {
                if( (frame->data[4] & 0x1F) != m_ModuleTypes[frame->data[0]])
                {
                    sprintf(Text, "  Fehler: %02X %02X %02X %02X", frame->data[4], frame->data[5], frame->data[6], frame->data[7] );
                    cout << Text << "\r\n";
                    m_o_state = 0;
                }
            }
            m_o_state = 4;
            break;

        case 10:
            if( m_o_state_count > 100 )
                m_o_state = 0;

            if( frame != NULL && frame->can_id == m_CanIdDi)
            {
                /* */
                m_o_state_count = 0;
            }

            /* operating */
            break;

        case 20:
            cout << "in error state "<< endl;
            if( frame != NULL )
                for(int i; i<8;i++)
                    cout<<" "<<frame->data[i];
            m_o_state = 0;
            break;

        case 30:
            break;

        default:
            break;
    }
}

void CanNode::InputStatemachine()
{

}


void CanNode::OuputStatemachine(CanIo *pIO,struct can_frame *frame)
{

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
        break;

        /* Node is in running state. eg. receiving and sending IO */
    case 100:
        /* extract DI */
        //CopySignalsFromImage( (uint64_t*) (frame->data));
        break;

    case 200:   /* enter error state */
        /* set io Off */
        // UpdateDigitalOutputs();
        break;

    default:
        m_state = 0;;
        break;
    }
}


