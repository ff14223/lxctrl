#ifndef MYSQLADAPTER_H
#define MYSQLADAPTER_H


#include "inc/IDatabase.h"

class MySqlAdapter : public IDatabase
{
     void *dbConn;		// the connection
     void CreateTables();
public:
     ~MySqlAdapter();
    MySqlAdapter(const char *, const char* );
    void  LogEntry(int,const char *);
    IBmzUser* getBmzUser(long id);
    void saveBmzUser(IBmzUser *pIUser);
};

#endif // MYSQLADAPTER_H
