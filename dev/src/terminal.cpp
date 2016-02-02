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
    sprintf(Text, "LoopTime: %04dms (%02d%%)  Sleep was: %04dms", t, (t*100)/250, t1 );
    cout << Text << endl;
    printsignals(pSystem);
}
