#ifndef IDATABASE_H
#define IDATABASE_H

#include "inc/IBmzUser.h"

class IDatabase {
public:
    virtual void  LogEntry(int,const char *)=0;
    virtual IBmzUser* getBmzUser(long id)=0;
    virtual void saveBmzUser(IBmzUser *pIUser)=0;
};

#endif // IDATABASE_H
