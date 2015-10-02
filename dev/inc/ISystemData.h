#ifndef ISYSTEMDATA_H
#define ISYSTEMDATA_H

#include "inc/IDatabase.h"
#include "inc/IDigitalSignal.h"

typedef struct
{
    IDatabase *pIDb;
}ISystemData;

typedef struct
{
    IDigitalSignal *pISDNBusStoerung;
}sSignalBMA;

typedef struct
{
    IDigitalSignal *pBmaAkkuFehler;
    IDigitalSignal *pBmaNetzFehler;
}sWarning;


typedef struct
{
    sSignalBMA bma;
    sWarning   warning;
}ISystemSignals;
#endif // ISYSTEMDATA_H
