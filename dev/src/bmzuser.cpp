#include <iostream>
#include "bmzuser.h"


bool BmzUser::isDisabled()
{
    return false;
}


long BmzUser::getBmaId()
{
    return m_BmaId;
}

std::string BmzUser::getName()
{
    return m_Name;
}

char BmzUser::getAlarmCondition()
{
    return m_AlarmCond;
}


std::string BmzUser::getAlarmConfiguration()
{
    return m_AlarmConf;
}

BmzUser::BmzUser(long bmauserid, string Name, long id,char AlarmCondition, string AlarmConfig) : DbObject(bmauserid)
{
    m_BmaId = id;
    m_icRoutineMissing = 0;
    m_Name = Name;
    m_AlarmCond = AlarmCondition;
    m_AlarmConf = AlarmConfig;
}
