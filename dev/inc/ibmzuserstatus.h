#ifndef IBMZUSERSTATUS
#define IBMZUSERSTATUS

#include <string>
using namespace std;

class IBmzUserStatus
{
public:
    virtual int getRoutineMissingCount()=0;
    virtual void setRoutineMissingCount(int value)=0;
    virtual string getStatus()=0;
    virtual void setStatus(string value)=0;

};

#endif // IBMZUSERSTATUS

