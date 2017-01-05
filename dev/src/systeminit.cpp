#include "inc/ISystemData.h"
#include <src/settings.h>
#include <src/ioimage.h>
#include <src/mysqladapter.h>
#include <src/alarm.h>
#include <src/alarmstatemachine.h>

void init(ISystem *pSystem)
{
    ISystemData *pSystemData = &(pSystem->Data);
    //
    // database connection
    //
    std::string dbUser = getSettings()->Cfg()->lookup("db.user");
    std::string dbPwd = getSettings()->Cfg()->lookup("db.pwd");
    std::string dbSchema = getSettings()->Cfg()->lookup("db.schema");
    pSystemData->pIDb = new MySqlAdapter( dbSchema.c_str(), dbUser.c_str(), dbPwd.c_str() );
    //
    // io image
    //
    pSystemData->pIo = new ioimage(pSystem);
    pSystemData->pAlarmStatemachine = new AlarmStateMachine(pSystem);
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

    pSignals->test.pInTest = pIo->getSignal("inTest");
}


void init_alarms(ISystem *pSystem)
{
    IAlarmStateMachine *pAlSm = pSystem->Data.pAlarmStatemachine;
    pSystem->Data.alarm.pStoerung = pAlSm->loadAlarm("Stoerungsalarm");
    pSystem->Data.alarm.pRoutineMissing = pAlSm->loadAlarm("FehlendeRoutine");
    pSystem->Data.alarm.pHausalarm = pAlSm->loadAlarm("Hausalarm");
}
