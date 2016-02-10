#include "inc/ISystemData.h"
#include "inc/IIoImage.h"



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

void ctrl_general(ISystem*pSystem)
{
    ctrl_ovd(pSystem);
}


void ctrl_alarm(ISystemData *pSystemData, ISystemSignals *pSignals)
{

}
