#include "mysqladapter.h"
#include "my_global.h"
#include <mysql.h>
#include <stdio.h>
using namespace std;


#define CON ((MYSQL*)dbConn)

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
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


void MySqlAdapter::CreateTables()
{
    /*
    CREATE TABLE `lxctrl`.`log` (
      `idlog` INT NOT NULL,
      `Erstellt` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,
      `Nr` INT NULL,
      `Text` VARCHAR(160) NULL,
      PRIMARY KEY (`idlog`)) */
}

