#ifndef ISYSTEMDATA_H
#define ISYSTEMDATA_H

#include "inc/IDatabase.h"
#include "inc/IDigitalSignal.h"
#include "inc/IIoImage.h"
#include "inc/IAlarm.h"


#define NR_OVD 8

typedef struct
{
    IAlarm *pHausalarm;
    IAlarm *pRoutineMissing;
    IAlarm *pStoerung;
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
    IDigitalSignal *pBmzCom1Offline;
}sWarning;

typedef struct
{
    IDigitalSignal *pInRaiseAlarm;
    IDigitalSignal *pInRaiseHausalarm;


}sAlarm;

typedef struct
{
    IDigitalSignal *pInOvdUp;
    IDigitalSignal *pInOvdDown;

    IDigitalSignal *pOutSigOvd[NR_OVD];
}sGeneral;

typedef struct
{
    sSignalBMA bma;
    sAlarm  alarm;
    sWarning   warning;
    sGeneral genral;
}ISystemSignals;


typedef struct
{
    unsigned long tloop;
    unsigned long tSleep;       /* time between activations */
    unsigned int uiCurrentOvdIndex;
}IValues;


typedef struct
{
    unsigned long CanFramesReceived;
    unsigned long CanFramesSent;
    unsigned long MainLoops;
    unsigned long BmzBytesReceived;
}ICounter;

typedef struct
{
    ISystemData    Data;
    ISystemSignals Signals;
    IValues        Values;
    ICounter       Counter;
}ISystem;



#endif // ISYSTEMDATA_H
