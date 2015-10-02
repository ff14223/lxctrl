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
};

#endif // MYSQLADAPTER_H
