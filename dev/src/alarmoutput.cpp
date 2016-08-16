#include "alarmoutput.h"

AlarmOutput::AlarmOutput(int tOn, int tDelayOn, IDigitalSignal *pSignal)
{
    this->tDelayOn = tDelayOn;
    this->tOn = tOn;
    this->pSignal = pSignal;
    iState = 0;
}

void AlarmOutput::setSignalRaiseAlarm(IDigitalSignal *pSignal)
{
    this->pSignal = pSignal;
}

void AlarmOutput::Raise()
{
    if( iState == 0 )
        iState = 1;
}

void AlarmOutput::Reset()
{
    iState = 0;
    if( pSignal )
        pSignal->set( false );
}

void AlarmOutput::StateMachine( int tElapsed )
{
    switch( iState )
    {
        case 0: /* idle */
            t = tDelayOn;
            break;

        case 1: /* wait tOn */
            if( t > tElapsed )
            {
                /* continue waiting */
                t -= tElapsed;
            }
            else
            {
                iState=2;
                t = tOn;
                if( pSignal )
                    pSignal->set( true );
            }
            break;

        case 2:
            if( t > tElapsed )
                t -= tElapsed;
            else
            {
                iState = 3;
                if( pSignal )
                    pSignal->set( false );
            }
            break;

        case 3: /* active */
            break;

        default:
            iState = 0;
            break;
    }
}
