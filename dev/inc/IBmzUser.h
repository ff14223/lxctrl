#ifndef IBMZUSER_H
#define IBMZUSER_H

#include <iostream>

using namespace std;

class IBmzUser
{
public:
    virtual long getBmaId()=0;
    virtual bool isDisabled()=0;
    virtual string getName()=0;
    virtual string getAlarmConfiguration()=0;
};

#endif // IBMZUSER_H
