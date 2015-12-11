#include <iostream>
#include "bmzuser.h"

long BmzUser::getObjectId(){return m_objectid;}
void BmzUser::setObjectId(long id){m_objectid=id;}

long BmzUser::getBmaId()
{
    return m_BmaId;
}

std::string BmzUser::getName()
{
    return m_Name;
}


int BmzUser::getRoutineMissingCount() { return m_icRoutineMissing;}
void BmzUser::setRoutineMissingCount(int value ){m_icRoutineMissing=value;}

BmzUser::BmzUser(string Name, long id)
{
    m_BmaId = id;
    m_icRoutineMissing = 0;
    m_Name = Name;
}
