#include "ioimage.h"
#include "exception"
#include "string"
#include "src/vds/digitalsignal.h"
#include "settings.h"
#include "iostream"

#include "inc/IDigitalSignal.h"

using namespace  std;
using namespace libconfig;

class SignalNotFoundException : public exception
{
    string s1;
    virtual const char* what() const throw()
    {
        // s1.append("Einstellungen konnten nicht geladen werden");
        return s1.c_str();
    }
public:
    void setReason(string reason){s1=reason;}
    virtual ~SignalNotFoundException() throw() { }
} SignalNotFound;


void ioimage::UpdateInputs()
{
    m_pCanIo->Input();
}

void ioimage::UpdateOutputs()
{
    m_pCanIo->Output();
}

IIoImage* getIOImage()
{
    static ioimage* image=0;
    if( image == 0 )
        image = new ioimage();
    return image;
}

ioimage::ioimage()
{
    m_pCanIo = new CanIo();

    m_pCanIo->GenerateSignals( &m_mapSignal );

    GenerateInternalSignals();      // internal Signals

    /*
     * Load signals from settings
    */
    const Setting &s = getSettings()->get("SIGNAL");
    int count = s.getLength();
    cout << "Lade " << count << " Signale..." << endl;
    for(int i = 0; i < count; ++i)
    {
          const Setting &signal = s[i];
          string signalName, signalMap;

          signal.lookupValue("name", signalName );
          signal.lookupValue("map", signalMap);

          MakeSignal(signalName, signalMap);
    }
}

void ioimage::GenerateInternalSignals()
{
    int SignalCount = 99;
    char text[100];
    m_DigitalSignal = new bool[SignalCount];
    cout << "Generiere " << SignalCount << " interne Signale." << endl;
    for(int i = 0; i < SignalCount; i++ )
    {
        DigitalSignal * d = new DigitalSignal( &m_DigitalSignal[i] );

        sprintf(text,"intern.sig[%d]",i);
        std::string SignalName = text;
        m_mapSignal[SignalName] = (IDigitalSignal*)d;
    }
}

void ioimage::DumpSignals()
{
      char Text[255];

      typedef std::map<std::string, IDigitalSignal*>::iterator it_type;
      for(it_type iterator = m_mapSignal.begin(); iterator != m_mapSignal.end(); iterator++)
      {
           bool state = iterator->second->get();
           sprintf(Text,"%s:%d",iterator->first.c_str(), state);
           cout << Text << endl;
      }
}

void ioimage::MakeSignal(std::string SignalName, std::string SignalMap)
{
    cout << "+ " << SignalName << "  "  << SignalMap << endl;

    IDigitalSignal *d = m_mapSignal[SignalMap];
    if( d == 0 )
    {
        SignalNotFound.setReason("Signal konnte nicht verbunden werden - " + SignalName + " nach " + SignalMap);
        throw SignalNotFound;
    }

    m_mapSignal[SignalName] = d;
}

IDigitalSignal* ioimage::getSignal(const std::string SignalName)
{
    IDigitalSignal *d = m_mapSignal[SignalName];
    if( d == 0)
    {
        SignalNotFound.setReason( "ioimage::getSignal - Signal nicht gefunden '" + SignalName +"'");
        throw SignalNotFound;
    }
}



