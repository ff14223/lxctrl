#include "bmzuserstatus.h"

BmzUserStatus::BmzUserStatus(long bmauserid) : DbObject(bmauserid)
{
    m_Status = "unbekannt";
}

int BmzUserStatus::getRoutineMissingCount()
{
    return m_RoutineMissingCount;
}

void BmzUserStatus::setRoutineMissingCount(int value)
{
    m_RoutineMissingCount = value;
}

void BmzUserStatus::setStatus( string value)
{
    m_Status=value;
}

string BmzUserStatus::getStatus()
{
    return m_Status;
}
