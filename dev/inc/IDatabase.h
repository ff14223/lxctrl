#ifndef IDATABASE_H
#define IDATABASE_H

#include "inc/IBmzUser.h"
#include "inc/ibmzuserstatus.h"

class IDatabase {
public:
    virtual void  LogEntry(int,const char *)=0;
    virtual IBmzUser* getBmzUser(long id)=0;
    virtual IBmzUserStatus*getBmzUserStatus(IBmzUser *pIUser)=0;
    virtual void saveBmzUserStatus(IBmzUserStatus *pIUserStatus)=0;
};

#endif // IDATABASE_H
