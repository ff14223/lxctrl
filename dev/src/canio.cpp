#include <iostream>

#include "canio.h"

#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "settings.h"

using namespace std;



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
    }
}

int CanIo::Test()
{
    struct can_frame frame;
    int ret;
    ret = recv(m_Socket, &frame, sizeof(frame), 0);
    if (ret != sizeof(frame))
    {
        if (ret < 0)
            perror("recv failed");
        else
            fprintf(stderr, "recv returned %d", ret);
        return -1;
    }
    cout << " FRAME RECEIVED\n" << endl;
    return 0;
}

CanIo::CanIo()
{
    LoadSettings();

    char *intf_name="can0";
    int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;

    printf("interface = %s, family = %d, type = %d, proto = %d\n",
           intf_name, family, type, proto);

    if ((m_Socket = socket(family, type, proto)) < 0)
    {
        cout << "CAN could not create socket" << endl;
    }
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

