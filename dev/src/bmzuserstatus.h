#ifndef BMZUSERSTATUS_H
#define BMZUSERSTATUS_H

#include "src/dbobject.h"
#include "inc/ibmzuserstatus.h"

class BmzUserStatus : public DbObject, public IBmzUserStatus
{
    string m_Status;
    int m_RoutineMissingCount;
public:
    BmzUserStatus(long bmauserid);
    int getRoutineMissingCount();
    void setRoutineMissingCount(int value);
    string getStatus();
    void setStatus(string value);
};

#endif // BMZUSERSTATUS_H
