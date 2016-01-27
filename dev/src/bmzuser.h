#ifndef BMZUSER_H
#define BMZUSER_H


#include "inc/IBmzUser.h"
#include "src/dbobject.h"

using namespace std;


class BmzUser : public DbObject, public IBmzUser
{
    string m_Name;
    long m_BmaId;
    int m_icRoutineMissing;
    char m_AlarmCond;
    string m_AlarmConf;

public:
    BmzUser(long bmauserid, string name, long id, char AlarmCondition, string AlarmConfig);
    virtual long getBmaId();
    virtual string getName();
    int getRoutineMissingCount();
    void setRoutineMissingCount(int value );
    char getAlarmCondition();
    virtual string getAlarmConfiguration();
    virtual bool isDisabled();
};

#endif
