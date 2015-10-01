#include "digitalsignal.h"

DigitalSignal::DigitalSignal(bool *value)
{
    if( value )
        m_value = value;
    else
        m_value = new bool[1];
}

bool DigitalSignal::get()
{
}

void DigitalSignal::set(bool value)
{
}
