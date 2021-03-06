#include <iostream>

#include "canio.h"

#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "settings.h"
#include <errno.h>
#include <fcntl.h>
#include <memory.h>


#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>




using namespace std;



/* translate CANId to NodeNr */
int getNodeNrCANId(int id)
{
    if( id >= 1566 && id < (1566 + 62) )
        return id - 1566 + 1;

    if( id>=286 && id <(286+31*4) )
        return (id-286) / 4+1;

    if( id >= 1630 && id < (1630+62) )
        return id - 1630 + 1;
    if( id >= 222 && id < (222+62) )
	return id - 222 +1;
    return 0;
}


/* Receive CAN Frame
 * translate CANID to Node Number
 * Pass CAN Frame to Node
 */
void CanIo::Input()
{
    struct can_frame frame;

    while( Receive(&frame) == 0 )
    {
	// 2022 = Identify Node, for CAN-IO etc
	if( frame.can_id == 0x2022 )	
            continue;

        int NodeNr = getNodeNrCANId( frame.can_id );
        if( NodeNr > 0)
        {
            cout << " Frame Received Id:" << frame.can_id << " for Node " << NodeNr << "\r\n";
            CanNode *pNode = m_mapNodes[NodeNr];
            if( pNode )
            {
                pNode->Statemachine(this, &frame );
                pNode->SetUpdated( true );
            }
            else
                cout<<"No such node" << NodeNr << "\r\n";
        }
        else
            cout << "CanId konnte keiner Node zugeordnet werden CAN-ID:" << frame.can_id << "\r\n";
    }

    std::map<int,CanNode*>::iterator it;

    /* call state machine for nodes width no frames received */
    for(it=m_mapNodes.begin(); it != m_mapNodes.end(); ++it)
    {
        if( it->second->GetUpdated() == false )
            it->second->Statemachine(this, NULL );

        it->second->UpdateDigitalInputs();
        it->second->SetUpdated(false);
    }
}

void CanIo::Output()
{
    std::map<int,CanNode*>::iterator it;
    /* call state machine for nodes width no frames received */
    for(it=m_mapNodes.begin(); it != m_mapNodes.end(); ++it)
    {
        it->second->UpdateDigitalOutputs(this);
    }
}


void CanIo::DumpInfo()
{
    cout << endl << "IO-IMAGE" << "\r\n";
    std::map<int,CanNode*>::iterator it = m_mapNodes.begin();
    for( ; it != m_mapNodes.end(); ++it)
        it->second->DumpInfo();
}

void CanIo::GenerateSignals(ioimage*image)
{
    std::map<int,CanNode*>::iterator it = m_mapNodes.begin();

    for( ; it != m_mapNodes.end(); ++it)
        it->second->GenerateSignals(image);
}

void CanIo::LoadSettings()
{
    const Setting &s = getSettings()->get("CAN");

    int iBaudRate;

    s.lookupValue("baud", iBaudRate );
    s.lookupValue("device", m_strDevice);

    cout << "CAN @device " << m_strDevice << " width " << iBaudRate << " kBit/s" << endl;

    const Setting &s2 = getSettings()->get("CAN.nodes");
    int count = s2.getLength();
    for(int i = 0; i < count; ++i)
    {
          const Setting &signal = s2[i];
          string Name;
          int iNodeNr;
          signal.lookupValue("node_nr", iNodeNr );
          signal.lookupValue("name", Name);

          cout << "    adding can node " << iNodeNr << " as " << Name << endl;
          m_mapNodes[iNodeNr] = new CanNode( iNodeNr, Name );
    }
}

int CanIo::Send(struct canfd_frame  *frame)
{
        frame->len = 4;

        /* all frames do have the same size (8Byte) */
        // toto repeat in case of error !!
        if( write( m_Socket, frame, 16 ) != 16)
             return 1;
        m_pSystem->Counter.CanFramesSent++;
        return 0;
}

int CanIo::Receive(struct can_frame *frame)
{
    int ret;
    ret = recv(m_Socket, frame, sizeof(*frame), 0);
    if (ret != sizeof(*frame))
    {
        /*if (ret < 0)
            perror("recv failed");
        else
            fprintf(stderr, "recv returned %d", ret);*/
        return -1;
    }
    m_pSystem->Counter.CanFramesReceived++;
    return 0;
}

CanIo::CanIo(ISystem*pSystem)
{
    m_pSystem = pSystem;
    LoadSettings();

    const char *intf_name="can0";
    int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;

    printf("interface = %s, family = %d, type = %d, proto = %d\n",
           intf_name, family, type, proto);

    /* open socket */
    if ((m_Socket  = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("socket");
    }



    struct sockaddr_can addr;


    addr.can_family = PF_CAN;
    addr.can_ifindex = 0; /* any can interface */

    /*memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    strncpy(ifr.ifr_name, ptr, nbytes);*/

    struct ifreq ifr;
    strcpy(ifr.ifr_name, m_strDevice.c_str() );
    if( ioctl(m_Socket, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("SIOCGIFINDEX");
    }
    addr.can_ifindex = ifr.ifr_ifindex;


    if( bind( m_Socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
    }

    /* we are the only master, so receive all packets */
    struct can_filter filter;
    filter.can_id = 0;
    filter.can_mask = 0;
    setsockopt( m_Socket, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter) );


    fcntl(m_Socket, F_SETFL, O_NONBLOCK);
}


/*Objekt
Berechnung
kn ma
Alarmmeldung         222 + (kn - 1)                         1 - 32 ----
digitale Eingänge    286 + (kn - 1) x 4                     1 - 32 ----
digitale Ausgänge    414 + (kn - 1) x 4                     1 - 32 ----
analoge Eingänge     542 + (kn - 1) x 16 + (ma - 1) x 4     1 - 32 1 - 4
analoge Ausgänge    1054 + (kn - 1) x 16 + (ma - 1) x 4     1 - 32 1 - 4
Befehlsanforderung  1566 + (kn - 1)                         1 - 63 ----
Befehlsantwort      1630 + (kn - 1)                         1 - 63 ----
*/

