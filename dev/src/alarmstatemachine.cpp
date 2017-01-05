#include "alarmstatemachine.h"
#include "src/alarmoutput.h"

#include "settings.h"
#include "src/alarm.h"

class AlarmException : public exception
{
    string s1;
    virtual const char* what() const throw()
    {
        // s1.append("Einstellungen konnten nicht geladen werden");
        return s1.c_str();
    }
public:
    void setReason(string reason){s1=reason;}
    virtual ~AlarmException() throw() { }
} AlarmException;

void AlarmStateMachine::UpdateStates(int tEllapsed)
{
    std::map<std::string,IAlarmOutput*>::iterator it;

    for(it=m_mapAlarmOutput.begin(); it != m_mapAlarmOutput.end();++it)
        it->second->StateMachine( tEllapsed );
}


void AlarmStateMachine::LoadConfiguration()
{
    const Setting &s = getSettings()->get("AlarmOutput");
    int count = s.getLength();
    cout << "Lade " << count << " Alarm Ausgänge..." << endl;
    for(int i = 0; i < count; ++i)
    {
          const Setting &signal = s[i];
          
          string Name, SignalName;
          int tOn, tDelayOn;

          signal.lookupValue("name", Name );
          signal.lookupValue("sigRaise", SignalName);
          signal.lookupValue("tOn", tOn );
          signal.lookupValue("tDelayOn", tDelayOn );

          /* check values */
          if( tOn > 10000 || tDelayOn > 10000 )
          {
              AlarmException.setReason("tOn oder tDelay on > 10000");
              throw AlarmException;
          }

          IDigitalSignal *pSignal =pSystem->Data.pIo->getSignal( SignalName);

          IAlarmOutput *pOutput = (IAlarmOutput*) new AlarmOutput(tOn,tDelayOn, pSignal);

          m_mapAlarmOutput[Name] = pOutput;
    }
}

AlarmStateMachine::AlarmStateMachine(ISystem *pSystem)
{
    this->pSystem = pSystem;
    LoadConfiguration();
}


IAlarmOutput* AlarmStateMachine::getAlarmOutput(string name)
{
    IAlarmOutput *result = m_mapAlarmOutput[name];
    if( result == NULL )
    {
        AlarmException.setReason("AlarmOutput nicht gefunden " + name);
        throw AlarmException;
    }

    return result;
}

IAlarm* AlarmStateMachine::loadAlarm(string name)
{
    IAlarm * pAlarm = m_mapAlarm[name];
    if( pAlarm != NULL )
        return pAlarm;


    cout << "Lade Alarm " << name << endl;

    /* laden und speichern */
    pAlarm = (IAlarm*) new Alarm( name, &pSystem->Data);
    if( pAlarm == NULL )
    {
        AlarmException.setReason("Alarm nicht gefunden " + name);
        throw AlarmException;
    }

    m_mapAlarm[name] = pAlarm;
    return pAlarm;
}

IAlarm* AlarmStateMachine::getAlarm(string name)
{
    IAlarm * pAlarm = m_mapAlarm[name];
    if( pAlarm == NULL )
    {
        AlarmException.setReason("Alarm nicht gefunden " + name);
        throw AlarmException;
    }
    return pAlarm;
}


