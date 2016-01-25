#include "alarm.h"


#include <src/settings.h>

Alarm::Alarm(const char *Name, ISystemData *pISystemData)
{
    const Setting &setting = getSettings()->get(Name);
    setting.lookupValue("id", m_id );
    std::string signal_name;
    setting.lookupValue("signal", signal_name);
    m_pSignal = pISystemData->pIo->getSignal(signal_name);
    m_pSystemData = pISystemData;
}

void Alarm::raise()
{
    cout << "Raise Alarm" << endl;
    m_pSignal->set( true );
    m_pSystemData->pIDb->LogEntry( m_id, "Raise Alarm" );
}
