#ifndef MYSQLADAPTER_H
#define MYSQLADAPTER_H


#include "inc/IDatabase.h"
#include "inc/ibmzuserstatus.h"
#include "inc/IBmzUser.h"

class MySqlAdapter : public IDatabase
{
     void *dbConn;		// the connection
     void CreateTables();
public:
     ~MySqlAdapter();
    MySqlAdapter(const char *, const char *, const char* );
    void  LogEntry(int,const char *);
    IBmzUser* getBmzUser(long id);
    void saveBmzUserStatus(IBmzUserStatus *pIUserStatus);
    bool TableExists(const char *Table);
    IBmzUserStatus*getBmzUserStatus(IBmzUser *pIUser);
};

#endif // MYSQLADAPTER_H
