#include "alarm.h"
#include <src/settings.h>

Alarm::Alarm(std::string Name, ISystemData *pISystemData)
{

    const Setting &setting = getSettings()->get(Name.c_str());
    setting.lookupValue("id", m_id );
    std::string signal_name;

    setting.lookupValue("signal", signal_name);
    m_pSignal = pISystemData->pIo->getSignal(signal_name);


    setting.lookupValue("text", m_text);

    try
    {
        setting.lookupValue("raise", signal_name);
        m_pRaise = pISystemData->pIo->getSignal(signal_name);
    }catch(exception e) {}

    m_pSystemData = pISystemData;


    const Setting &s1 = getSettings()->get((Name + ".outputs").c_str());
    int count = s1.getLength();
    for(int i = 0; i < count; ++i)
    {
        int delay=100;
        std:string output;

        s1[i].lookupValue("delay", delay);
        s1[i].lookupValue("output", output);

        AddOutput( delay, pISystemData->pAlarmStatemachine->getAlarmOutput(output));
    }
}

void Alarm::reset()
{
    for (vector<IAlarmOutput*>::iterator it = vAlarmOutputs.begin() ;
         it != vAlarmOutputs.end(); ++it)
    {
       (*it)->Reset();
    }
    m_pSignal->set(false);
    char Data[250];
    sprintf(Data,m_text.c_str(), "zurückgesetzt");
    m_pSystemData->pIDb->LogEntry( m_id, Data );
}

void Alarm::raise()
{
    char Data[250];
    sprintf(Data,m_text.c_str(), "ausgelöst");
    m_pSignal->set( true );
    m_pSystemData->pIDb->LogEntry( m_id, Data );

    for (std::vector<IAlarmOutput*>::iterator it = vAlarmOutputs.begin() ;
         it != vAlarmOutputs.end(); ++it)
    {
       (*it)->Raise();
    }
}

void Alarm::AddOutput(int n,IAlarmOutput* out)
{
    vAlarmOutputs.push_back( out );
}
