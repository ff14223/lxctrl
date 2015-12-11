#ifndef IBMZUSER_H
#define IBMZUSER_H

#include <iostream>

class IBmzUser
{
public:
    virtual long getBmaId()=0;
    virtual std::string getName()=0;
    virtual int getRoutineMissingCount()=0;
    virtual void setRoutineMissingCount(int value)=0;
};

#endif // IBMZUSER_H
