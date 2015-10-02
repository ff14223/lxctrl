#include "inc/ISystemData.h"
#include <src/settings.h>
#include <src/ioimage.h>
#include <src/mysqladapter.h>

void init(ISystemData *pSystemData)
{
    // database connection
    std::string dbUser = getSettings()->Cfg()->lookup("db.user");
    std::string dbPwd = getSettings()->Cfg()->lookup("db.pwd");
    pSystemData->pIDb = new MySqlAdapter( dbUser.c_str(), dbPwd.c_str() );
}

void init_signals(ISystemData *pSystemData, ISystemSignals *pSignals)
{
    ioimage &io = getIOImage();
    pSignals->bma.pISDNBusStoerung = io.getSignal("ISDNBusStoerung");
    pSignals->warning.pBmaAkkuFehler = io.getSignal("BmaAkkuFehler");
}
