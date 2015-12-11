#ifndef BMZUSER_H
#define BMZUSER_H


#include "inc/IBmzUser.h"


using namespace std;


class BmzUser : public IBmzUser
{
    string m_Name;
    long m_BmaId;
    int m_icRoutineMissing;
    long m_objectid;
public:
    BmzUser(string name, long id);
    virtual long getBmaId();
    virtual string getName();
    int getRoutineMissingCount();
    void setRoutineMissingCount(int value );
    long getObjectId();
    void setObjectId(long id);
};

#endif
