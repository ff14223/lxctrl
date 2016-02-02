#ifndef CANIO_H
#define CANIO_H

#include "src/cannode.h"

using namespace std;

class CanIo
{
    int m_Socket;
    string m_strDevice;
public:
    CanIo();
    void LoadSettings();
    int Test();
};

#endif // CANIO_H
