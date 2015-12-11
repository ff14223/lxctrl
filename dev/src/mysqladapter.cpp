#include <iostream>

#include "mysqladapter.h"
#include "my_global.h"
#include <mysql.h>
#include <stdio.h>
#include <string>
#include "src/bmzuser.h"

#define CON ((MYSQL*)dbConn)

using namespace std;

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

void MySqlAdapter::saveBmzUser(IBmzUser *pIUser)
{
    if( pIUser == NULL )
        return;

    BmzUser*pUser = static_cast<BmzUser*>(pIUser);

    char statement[1024];
    sprintf( statement, "update bmauser set cRoutineMissing='%d' where idbmauser='%ld'; ",
             pIUser->getRoutineMissingCount(), pUser->getObjectId() );

    if( mysql_query(CON, statement) )
    {
        printf("Error Updating User\n");
        finish_with_error(CON);
    }
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

    BmzUser* user = new BmzUser(row[3],id);
    user->setRoutineMissingCount( atoi( row[2]) );
    user->setObjectId( atol(row[0]) );
    user->setRoutineMissingCount( atoi(row[4]) );
    mysql_free_result(result);

    return (IBmzUser*) user;
}



MySqlAdapter::~MySqlAdapter()
{
    mysql_close(CON);
}

MySqlAdapter::MySqlAdapter(const char *username, const char *pwd)
{
    printf("MySql Client Version %s\n", mysql_get_client_info());

    dbConn = mysql_init(NULL);

    if(mysql_real_connect( CON, "localhost", username, pwd, "lxctrl", 0, NULL, 0) == NULL)
    {
        printf("No connection to Database\n");
        finish_with_error(CON);
    }

   // CreateTables(); // Make sure all tables are there
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


void MySqlAdapter::CreateTables()
{
    if( mysql_query(CON, "SELECT 1 FROM lxctrl LIMIT 1;") )
    {
        mysql_query(CON, "CREATE TABLE `lxctrl`.`log` ( "
                    "`idlog` INT NOT NULL,"
                    "`Erstellt` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,"
                    "`Nr` INT NULL,"
                    "`Text` VARCHAR(160) NULL,"
                    "PRIMARY KEY (`idlog`))");
    }

    if( mysql_query(CON, "SELECT 1 FROM bmauser LIMIT 1;") )
    {
        if( mysql_query(CON, "CREATE TABLE `lxctrl`.`bmauser` ( "
                    "`idbmauser` INT NOT NULL,"
                    "`Erstellt` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,"
                    "ID INT NULL,"
                    "`Name` VARCHAR(160) NULL,"
                    "'cRoutineMissing' int,"
                    "PRIMARY KEY (`idbmauser`))") )
        {
            printf("Tabelle 'bmauser' konnte nicht erzeugt werden.\n");
            finish_with_error(CON);
        }
    }
}

