#include "digitalsignal.h"

DigitalSignal::DigitalSignal(bool *value)
{
    m_IoValue = value;
    this->set( false );
    updateOutput();
}

bool DigitalSignal::raised()
{
    return m_Raised;
}

void DigitalSignal::updateOutput()
{
    m_Raised = false;
    *m_IoValue = m_CurrentValue;
}

// copy external value to internal
void DigitalSignal::updateInput()
{
    m_Raised = false;
    this->set( *m_IoValue );
}

bool DigitalSignal::get()
{
    return m_CurrentValue;
}

void DigitalSignal::set(bool value)
{
    if( m_CurrentValue == false && value == true )
        m_Raised = true;

    m_CurrentValue = value;
}
