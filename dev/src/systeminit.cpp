#include "inc/ISystemData.h"
#include <src/settings.h>
#include <src/ioimage.h>
#include <src/mysqladapter.h>
#include <src/alarm.h>

void init(ISystem *pSystem)
{
    ISystemData *pSystemData = &(pSystem->Data);
    // database connection
    std::string dbUser = getSettings()->Cfg()->lookup("db.user");
    std::string dbPwd = getSettings()->Cfg()->lookup("db.pwd");
    pSystemData->pIDb = new MySqlAdapter( dbUser.c_str(), dbPwd.c_str() );
    // io image
    pSystemData->pIo = new ioimage(pSystem);
}

void init_signals(ISystemData *pSystemData, ISystemSignals *pSignals)
{
    char Text[128];
    IIoImage *pIo = pSystemData->pIo;
    pSignals->bma.pISDNBusStoerung = pIo->getSignal("ISDNBusStoerung");
    pSignals->warning.pBmaAkkuFehler = pIo->getSignal("BmaAkkuFehler");
    pSignals->warning.pBmzCom1Offline = pIo->getSignal("BmzComOneOffline");
    pSignals->alarm.pInRaiseAlarm = pIo->getSignal("inRaiseAlarm");
    pSignals->alarm.pInRaiseHausalarm = pIo->getSignal("inHausAlarm");

    pSignals->genral.pInOvdUp = pIo->getSignal("inOvdUp");
    pSignals->genral.pInOvdDown = pIo->getSignal("inOvdDown");

    for(int i=0; i<NR_OVD;i++)
    {
        sprintf(Text, "outSigOvd%02d", i);
        pSignals->genral.pOutSigOvd[i]= pIo->getSignal(Text);
    }
}


void init_alarms(ISystem *pSystem)
{
    pSystem->Data.alarm.pStoerung = new Alarm("Stoerungsalarm", &(pSystem->Data));
    pSystem->Data.alarm.pRoutineMissing = new Alarm("FehlendeRoutine", &(pSystem->Data));
    pSystem->Data.alarm.pHausalarm = new Alarm("Hausalarm", &(pSystem->Data));
}
