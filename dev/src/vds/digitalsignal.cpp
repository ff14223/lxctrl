#include "digitalsignal.h"


void DigitalSignal::setSimulationMode(bool value)
{
    m_SimulationValue = false;
    m_Simulate = value;
}

bool DigitalSignal::getSimulationMode()
{
    return m_Simulate;
}

void DigitalSignal::setSimulationValue(bool value)
{
    m_SimulationValue = value;
}


DigitalSignal::DigitalSignal(bool *value)
{
    m_Simulate = false;
    m_SimulationValue = false;

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
    if( m_Simulate == false )
        *m_IoValue = get();
}

// copy external value to internal
void DigitalSignal::updateInput()
{
    m_Raised = false;
    if( m_Simulate == false )
        this->set( *m_IoValue );
    else
        this->set( m_SimulationValue );
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
