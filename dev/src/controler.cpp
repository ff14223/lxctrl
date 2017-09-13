#include "inc/ISystemData.h"
#include "inc/IIoImage.h"

#include <sys/file.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>

/*
 *  OVD Handling
 *
 * Eingänge - inOvdUp, in OvdDown
 *
 * Ausgänge
 *     sigOvd01..08
 */
void ctrl_ovd(ISystem*pSystem)
{
    int iCurrentOvd = pSystem->Values.uiCurrentOvdIndex;
    if( iCurrentOvd<(NR_OVD-1) &&
        pSystem->Signals.genral.pInOvdUp->raised() == true )
        iCurrentOvd++;

    if( iCurrentOvd>0 &&
        pSystem->Signals.genral.pInOvdDown->raised() == true  )
        iCurrentOvd--;

    for(int i=0;i<NR_OVD; i++)
    {
        pSystem->Signals.genral.pOutSigOvd[i]->set( i == iCurrentOvd );
    }

    pSystem->Values.uiCurrentOvdIndex = iCurrentOvd;

}

/*
 *
 *
*/
void ctrl_general(ISystem*pSystem)
{
    if( pSystem->Signals.test.pInTest->raised() )
    {
        pSystem->Data.alarm.pHausalarm->reset();
        pSystem->Data.alarm.pHausalarm->raise();
    }

    /* ovd funktionalität */
    ctrl_ovd(pSystem);
}

void ctrl_bma_device(sBma*pBma)
{
    char data[256];

    if( pBma->bDataAvailable == false)
        return;
    pBma->bDataAvailable = false;

    if( pBma->fd < 0 )
        return;

    int nrBytesRead = 0;

    ioctl(pBma->fd, FIONREAD, &nrBytesRead);
    if( nrBytesRead <= 0 )
        return;

    if( nrBytesRead > sizeof(data) )
        nrBytesRead = sizeof(data);

    nrBytesRead = read( pBma->fd, data, nrBytesRead );

    // Dmp BMA Data
    if( pBma->fdLog  > 0 )
        write( pBma->fdLog, data,nrBytesRead );

    for(int i=0;i<nrBytesRead;i++)
    {
        pBma->Vds.pVdsInput->ReceiveFrameStateMachine( data[i] );
        pBma->Vds.BmzBytesReceived++;
    }
}


void ctrl_bma(ISystem *pSystem)
{
    ctrl_bma_device( &(pSystem->BmaMain) );
    ctrl_bma_device( &(pSystem->BmaFailover) );
}


/*
 *
 *
*/
bool bAlarmRaised=false;
void ctrl_alarm_raise_from_key(ISystem *pSystem)
{
    if( bAlarmRaised == true )
        return;

    if( pSystem->Signals.alarm.pInRaiseHausalarm->get() == true )
    {
        pSystem->Data.alarm.pHausalarm->raise();
        bAlarmRaised = true;
    }

    // Todo add more alarms
}

void ctrl_alarm(ISystem *pSystem)
{
    /* Check if inputs are pressed */
    if( pSystem->Signals.alarm.pInRaiseAlarm->get() == true )
        ctrl_alarm_raise_from_key(pSystem);
    else
        bAlarmRaised = false;

    /* update AlarmStatemachine */
    int t = pSystem->Values.tSleep; /* time between calls */
    pSystem->Data.pAlarmStatemachine->UpdateStates( t );
}
