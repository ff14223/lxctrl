#include "inc/ISystemData.h"
#include <src/settings.h>
#include <src/ioimage.h>
#include <src/mysqladapter.h>
#include <src/alarm.h>

void init(ISystemData *pSystemData)
{
    // database connection
    std::string dbUser = getSettings()->Cfg()->lookup("db.user");
    std::string dbPwd = getSettings()->Cfg()->lookup("db.pwd");
    pSystemData->pIDb = new MySqlAdapter( dbUser.c_str(), dbPwd.c_str() );
    // io image
    pSystemData->pIo = getIOImage();
}

void init_signals(ISystemData *pSystemData, ISystemSignals *pSignals)
{
    IIoImage *pIo = pSystemData->pIo;
    pSignals->bma.pISDNBusStoerung = pIo->getSignal("ISDNBusStoerung");
    pSignals->warning.pBmaAkkuFehler = pIo->getSignal("BmaAkkuFehler");
    pSignals->alarm.pInRaiseAlarm = pIo->getSignal("inRaiseAlarm");
    pSignals->alarm.pInRaiseHausalarm = pIo->getSignal("inHausAlarm");
}


void init_alarms(ISystemData *pSystemData, ISystemSignals *pSignals)
{
    pSystemData->alarm.pStoerung = new Alarm();
    pSystemData->alarm.pRoutineMissing = new Alarm();
    pSystemData->alarm.pHausalarm = new Alarm();
}
