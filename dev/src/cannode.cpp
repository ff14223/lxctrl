#include "cannode.h"
#include <iostream>

#include "stdio.h"

#include "src/vds/digitalsignal.h"

void CanNode::GenerateSignals(std::map<std::string, IDigitalSignal*> *map)
{
    char text[100];


    for(int i = 0; i < 64; i++ )
    {
        DigitalSignal * d = new DigitalSignal( &m_DigitalInput[i] );

        sprintf(text,"%s.di[%d]",m_Name.c_str(),i);
        std::string SignalName = text;
        (*map)[SignalName] = (IDigitalSignal*)d;
    }

    for(int i = 0; i < 64; i++ )
    {
        DigitalSignal * d = new DigitalSignal( &m_DigitalOutput[i] );

        sprintf(text,"%s.do[%d]",m_Name.c_str(),i);
        std::string SignalName = text;
        (*map)[SignalName] = (IDigitalSignal*)d;
    }
}

CanNode::CanNode(int NodeNr, string Name)
{
    m_NodeNr = NodeNr;
    m_Name = Name;
}

