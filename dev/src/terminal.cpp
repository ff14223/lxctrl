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

    sprintf(Text, "BMZ %08ld bytes received",
            pSystem->Counter.BmzBytesReceived);
    cout << Text << "\r\n";

    printsignals(pSystem);
}
