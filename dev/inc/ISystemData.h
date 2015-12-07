#ifndef ISYSTEMDATA_H
#define ISYSTEMDATA_H

#include "inc/IDatabase.h"
#include "inc/IDigitalSignal.h"
#include "inc/IIoImage.h"
#include "inc/IAlarm.h"


typedef struct
{
    IAlarm *pHausalarm;
}sAlarms;

typedef struct
{
    IIoImage *pIo;
    IDatabase *pIDb;
    sAlarms alarm;
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
    IDigitalSignal *pInRaiseAlarm;
    IDigitalSignal *pInRaiseHausalarm;
}sAlarm;

typedef struct
{
    sSignalBMA bma;
    sAlarm  alarm;
    sWarning   warning;
}ISystemSignals;
#endif // ISYSTEMDATA_H
