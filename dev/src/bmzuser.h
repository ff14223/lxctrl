#ifndef BMZUSER_H
#define BMZUSER_H

#include <iostream>
#include "inc/IBmzUser.h"

using namespace std;

class BmzUser : public IBmzUser
{
    string Name;
    long m_bmaid;
public:
    BmzUser(string name, long id);
    long getBmaId();
};

#endif // BMZUSER_H
