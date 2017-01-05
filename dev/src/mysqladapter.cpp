#include <iostream>

#include "mysqladapter.h"
#include "my_global.h"
#include <mysql.h>
#include <stdio.h>
#include <string>

#include "src/bmzuser.h"
#include "src/bmzuserstatus.h"

#define CON ((MYSQL*)dbConn)

using namespace std;

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

void MySqlAdapter::saveBmzUserStatus(IBmzUserStatus *pIUserStatus)
{
    if( pIUserStatus == NULL )
        return;

    BmzUserStatus*pStatus = static_cast<BmzUserStatus*>(pIUserStatus);

    char statement[1024];
    int count1 = pIUserStatus->getRoutineMissingCount();
    int count2=0;
    string status=pStatus->getStatus();
    if( pStatus->getIsNew() )
        sprintf( statement,
                 "INSERT INTO `lxctrl`.`bmauserstatus` "
                 "(`idbmauser`,`accesses`,`status`,`count1`,`count2`) "
                 "VALUES(%ld,CURRENT_TIMESTAMP,'%s',%d,%d) ",
                 pStatus->getObjectId(),status.c_str(),count1,count2 );
    else
        sprintf( statement, "update bmauserstatus set count1='%d', "
                 "status='%s' where idbmauser='%ld'; ",
             pIUserStatus->getRoutineMissingCount(), status.c_str(),pStatus->getObjectId() );

    if( mysql_query(CON, statement) )
    {
        printf("Error Updating Status\n");
        finish_with_error(CON);
    }
}

IBmzUserStatus* MySqlAdapter::getBmzUserStatus(IBmzUser *pIUser)
{
    if( pIUser == NULL )
        return NULL;

    BmzUser *pUser = static_cast<BmzUser*>(pIUser);
    BmzUserStatus *status = new BmzUserStatus(pUser->getObjectId() );
    status->setIsNew(true);

    char statement[255];
    sprintf(statement, "select * from bmauserstatus where idbmauser='%ld'", pUser->getObjectId());
    if (mysql_query(CON, statement))
          return status;

    MYSQL_RES *result = mysql_store_result(CON);
    MYSQL_ROW row = mysql_fetch_row(result);
    if( row == NULL )
        return status;

    int num_fields = mysql_num_fields(result);

    if( row == NULL )
        return status;    // return new object

    status->setIsNew( false );

    return status;
}

IBmzUser* MySqlAdapter::getBmzUser(long id)
{
    char statement[255];
    sprintf(statement, "select * from bmauser where ID='%ld'", id);
    if (mysql_query(CON, statement))
          return NULL;

    MYSQL_RES *result = mysql_store_result(CON);

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row = mysql_fetch_row(result);
    if( row == NULL )
        return NULL;

    for(int i = 0; i < num_fields; i++)
    {
        printf("%s ", row[i] ? row[i] : "NULL");
    }


    char AlarmCond = row[5][0];
    string AlarmConfiguration = row[6];
    long bmauserid = atoi(row[0]);
    BmzUser* user = new BmzUser(bmauserid, row[3], id, AlarmCond, AlarmConfiguration);
    mysql_free_result(result);

    return (IBmzUser*) user;
}



MySqlAdapter::~MySqlAdapter()
{
    mysql_close(CON);
}

MySqlAdapter::MySqlAdapter(const char * schema, const char *username, const char *pwd)
{
    printf("MySql Client Version %s\n", mysql_get_client_info());

    dbConn = mysql_init(NULL);

    if(mysql_real_connect( CON, "localhost", username, pwd, schema, 0, NULL, 0) == NULL)
    {
        printf("No connection to Database\n");
        finish_with_error(CON);
    }

    CreateTables(); // Make sure all tables are there
}




void MySqlAdapter::LogEntry(int Type, const char *Text)
{
    char statement[255];
    sprintf(statement, "INSERT INTO log (Nr,Text) VALUES(%d,'%s')", Type, Text);
    if (mysql_query(CON, statement)) {
          printf("Log Eintrag konnte nicht erstellt werden.\n");
          finish_with_error(CON);
      }
}

bool MySqlAdapter::TableExists(const char *Table)
{
     //MYSQL_RES *result = mysql_store_result(CON);
     char Query[255];
     bool bresult=true;
     sprintf(Query, "SELECT * FROM information_schema.tables  WHERE table_schema = 'lxctrl' "
                    "AND table_name = '%s' LIMIT 1", Table);


     mysql_query(CON, Query);
     MYSQL_RES *result = mysql_store_result(CON);
     if( result == NULL || result->row_count == 0)
     {
        bresult=false;
        cout << "Table not found " << Table << endl;
     }

     mysql_free_result(result);

     return bresult;
}

void MySqlAdapter::CreateTables()
{
    if( TableExists("log") == false )
    {
        mysql_query(CON, "CREATE TABLE `lxctrl`.`log` ( "
                    "`idlog` INT NOT NULL AUTO_INCREMENT,"
                    "`Erstellt` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,"
                    "`Nr` INT NULL,"
                    "`Text` VARCHAR(160) NULL,"
                    "PRIMARY KEY (`idlog`))");
    }

    if( TableExists("bmauser") == false )
    {
        if( mysql_query(CON, "CREATE TABLE `lxctrl`.`bmauser` ( "
                    "`idbmauser` int(11) NOT NULL AUTO_INCREMENT,"
                    "`Erstellt` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,"
                    "ID INT NULL,"
                    "`Name` VARCHAR(160) NULL,"
                    "`cRoutineMissing` int,"
                    "`AlarmCond` VARCHAR(1) NULL,"
                    "`AlarmConfig` VARCHAR(255) NULL,"
                    "PRIMARY KEY (`idbmauser`))") )
        {
            printf("Tabelle 'bmauser' konnte nicht erzeugt werden.\n");
            finish_with_error(CON);
        }
    
        /*
         * Create Default User
        */
        if( mysql_query(CON, "insert into lxctrl.bmauser "
                        "(`idbmauser`,`Erstellt`,`ID`,`Name`,`cRoutineMissing`,`AlarmCond`,`AlarmConfig`) "
                        "VALUES(0,CURRENT_TIMESTAMP,0,'Default User',0,'F','AL')" ) )
        {
            printf("Default BMA User konnte nicht angelegt werden.\n");
            finish_with_error(CON);
        }
    }

    if( TableExists("bmauserstatus") == false )
    {
        if( mysql_query(CON, "CREATE TABLE `lxctrl`.`bmauserstatus` ( "
                    "`idbmauser` int(11) NOT NULL,"
                    "`accesses` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,"
                    "`status` VARCHAR(80) NULL,"
                    "`count1` int,"
                    "`count2` int,"
                    "PRIMARY KEY (`idbmauser`))") )
        {
            printf("Tabelle 'bmauserstatus' konnte nicht erzeugt werden.\n");
            finish_with_error(CON);
        }
    }




    
}

