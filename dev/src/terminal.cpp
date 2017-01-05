#include "inc/ISystemData.h"
#include "stdio.h"

void printsignals(ISystem *pSystem)
{
    pSystem->Data.pIo->DumpSignals();
}

void printpage(ISystem *pSystem)
{
    printf("\033[2J\033[1;1H\n");
    char Text[80];
    int t = pSystem->Values.tloop;
    int t1 = pSystem->Values.tSleep;
    sprintf(Text, "Loop  %08ld t: %04dms (%02d%%)  Sleep was: %04dms",
            pSystem->Counter.MainLoops,
            t, (t*100)/250, t1 );
    cout << Text << "\r\n";


    sprintf(Text, "CAN Frames received:%08ld sent:%08ld",
            pSystem->Counter.CanFramesReceived,
            pSystem->Counter.CanFramesSent);
    cout << Text << "\r\n";


    sprintf(Text, "BMZ Main %08ld bytes received",
            pSystem->BmaMain.Vds.BmzBytesReceived);
    cout << Text << "\r\n";

    sprintf(Text, "BMZ Failover %08ld bytes received",
            pSystem->BmaFailover.Vds.BmzBytesReceived);
    cout << Text << "\r\n";

    sprintf(Text, "VdS Frames received:%08ld errors:%08ld",
            pSystem->BmaMain.Vds.pVdsInput->getFrameReceiveCount(),
            pSystem->BmaMain.Vds.pVdsInput->getFrameErrorCount());
    cout << Text << "\r\n";

    printsignals(pSystem);


    /* Some important values */

    cout << "\r\n";
    cout << "Keys:'1'/'0'' Simulation. 'q' to close..." << "\r\n";
}
