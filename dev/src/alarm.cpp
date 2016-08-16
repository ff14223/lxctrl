#include "alarm.h"


#include <src/settings.h>

Alarm::Alarm(std::string Name, ISystemData *pISystemData)
{
    const Setting &setting = getSettings()->get(Name.c_str());
    setting.lookupValue("id", m_id );
    std::string signal_name;
    setting.lookupValue("signal", signal_name);
    m_pSignal = pISystemData->pIo->getSignal(signal_name);
    m_pSystemData = pISystemData;

    const Setting &s1 = getSettings()->get((Name + ".outputs").c_str());
    int count = s1.getLength();
    for(int i = 0; i < count; ++i)
    {
          const char *s = setting["outputs"][i];
          AddOutput( pISystemData->pAlarmStatemachine->getAlarmOutput(s));
    }

}

void Alarm::reset()
{
    for (std::vector<IAlarmOutput*>::iterator it = vAlarmOutputs.begin() ;
         it != vAlarmOutputs.end(); ++it)
    {
       (*it)->Reset();
    }
    m_pSignal->set(false);
    m_pSystemData->pIDb->LogEntry( m_id, "Alarm zurückgesetzt." );
}

void Alarm::raise()
{
    m_pSignal->set( true );
    m_pSystemData->pIDb->LogEntry( m_id, "Alarm ausgelöst." );

    for (std::vector<IAlarmOutput*>::iterator it = vAlarmOutputs.begin() ;
         it != vAlarmOutputs.end(); ++it)
    {
       (*it)->Raise();
    }
}

void Alarm::AddOutput(IAlarmOutput* out)
{
    vAlarmOutputs.push_back( out );
}
