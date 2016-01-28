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
    int t = pSystem->Values.tloop / 1000 ;
    sprintf(Text, "LoopTime: %04dms (%02d%%)", t, (t*100)/250 );
    cout << Text << endl;
    printsignals(pSystem);
}
