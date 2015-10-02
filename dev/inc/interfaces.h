#ifndef INTERFACES_H
#define INTERFACES_H

#include "inc/IDatabase.h"

class Interfaces
{
    IDatabase *pIDb;
    public:
    Interfaces(IDatabase* pIDb){this->pIDb = pIDb;}
};

#endif // INTERFACES_H
