#include "inc/ISystemData.h"
#include "inc/IIoImage.h"

void ctrl_general(ISystemData *pSystemData, ISystemSignals *pSignals)
{

}


void ctrl_alarm(ISystemData *pSystemData, ISystemSignals *pSignals)
{
    if( pSignals->alarm.pInRaiseAlarm->get() == true )
    {
        if( pSignals->alarm.pInRaiseHausalarm->raised() )
            pSystemData->alarm.pHausalarm->raise();
    }
}
