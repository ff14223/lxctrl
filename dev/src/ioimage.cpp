#include "ioimage.h"
#include "exception"
#include "string"
#include "src/vds/digitalsignal.h"
#include "settings.h"
#include "iostream"

#include "inc/IDigitalSignal.h"
#include "inc/termcolor.h"

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

int ioimage::getNrSimulationMappings()
{
    return m_mapKeyToSignal.size();
}

void ioimage::UpdateInputs()
{
    typedef std::map<std::string, IDigitalSignal*>::iterator it_type;
    for(it_type iterator = m_mapActiveSignal.begin(); iterator != m_mapActiveSignal.end(); iterator++)
        iterator->second->updateInput();

    m_pCanIo->Input();
}

void ioimage::UpdateOutputs()
{
    typedef std::map<std::string, IDigitalSignal*>::iterator it_type;
    for(it_type iterator = m_mapActiveSignal.begin(); iterator != m_mapActiveSignal.end(); iterator++)
        iterator->second->updateOutput();

    m_pCanIo->Output();
}

void ioimage::GenerateSignals()
{
    m_pCanIo->GenerateSignals( this );

    GenerateInternalSignals();      // internal Signals
    CheckSignals();
}


ioimage::ioimage(ISystem *pSystem)
{
    m_pSystem = pSystem;
    m_pCanIo = new CanIo(pSystem);




    GenerateSignals();

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

          /* Test if Signal Mapping allready used */
          string value = m_mapMappingSignal[signalMap];
          if( value.empty() ==false  )
          {
              SignalNotFound.setReason( "ioimage::Ausgang wird bereits verwendet '" + signalMap + "'"
                                         + "von " + value);
              throw SignalNotFound;
          }
          m_mapMappingSignal[signalMap] = signalName;


          MakeSignal(signalName, signalMap);
    }

    const Setting &s1 = getSettings()->get("SIMULATE");
    count = s1.getLength();
    cout << "Lade " << count << " SIMULATIONEN..." << endl;
    for(int i = 0; i < count; ++i)
    {
          const Setting &signal = s1[i];
          string signalName, signalKey;
          signal.lookupValue("signal", signalName );
          signal.lookupValue("key", signalKey);
          m_mapKeyToSignal[signalKey] = signalName;
    }
}

void ioimage::AddSignal(string SignalName, IDigitalSignal *pSignal)
{
    IDigitalSignal *d = m_mapSignal[SignalName];
    if( d != NULL )
    {
        SignalNotFound.setReason("Signal wird bereits verwendet- " + SignalName);
        throw SignalNotFound;
    }

    m_mapSignal[SignalName] = pSignal;
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
        AddSignal(SignalName, d );
    }
}

void ioimage::CheckSignals()
{
}

void ioimage::KeyPressed(char keyPressed)
{
    char Text[]={0,0};
    Text[0] = keyPressed;
    std::string key=Text;
    string SignalName = m_mapKeyToSignal[key];


    IDigitalSignal *pSignal = m_mapSignal[SignalName];
    if( pSignal )
    {
        if( pSignal->getSimulationMode() == true )
            pSignal->setSimulationMode(false);
        else
            pSignal->setSimulationMode(true);
    }

    typedef std::map<std::string, string>::iterator it_type;

    if( keyPressed == '1' )
    {
        for(it_type iterator = m_mapKeyToSignal.begin(); iterator != m_mapKeyToSignal.end(); iterator++)
        {
            IDigitalSignal *pSignal = m_mapSignal[iterator->second];
            if( pSignal == NULL || pSignal->getSimulationMode() ==false)
                continue;

            pSignal->setSimulationValue( true );
        }
    }

    if( keyPressed == '0')
    {
        for(it_type iterator = m_mapKeyToSignal.begin(); iterator != m_mapKeyToSignal.end(); iterator++)
        {
            IDigitalSignal *pSignal = m_mapSignal[iterator->second];
            if( pSignal == NULL || pSignal->getSimulationMode() ==false)
                continue;

            pSignal->setSimulationValue( false );
        }
    }
}

void ioimage::DumpSignals()
{

      Color::Modifier red( Color::FG_RED );
      Color::Modifier green( Color::FG_GREEN);
      Color::Modifier simulation( Color::BG_BLUE);
      Color::Modifier def( Color::FG_DEFAULT);
      Color::Modifier bg_def( Color::BG_DEFAULT);

      char Text[255];

      m_pCanIo->DumpInfo();

      typedef std::map<std::string, IDigitalSignal*>::iterator it_type;
      int i=0;
      for(it_type iterator = m_mapActiveSignal.begin(); iterator != m_mapActiveSignal.end(); iterator++)
      {
           bool state = iterator->second->get();
           sprintf(Text,"%18s: ",iterator->first.c_str(), state);
           if( iterator->second->getSimulationMode() == true )
               cout << simulation;

           cout << Text << bg_def;

           if( state == true )
                cout << red << state << def;
           else
                cout << green << state << def;
           i++;
           if( (i % 3) == 0  )
               cout << "\r\n";

      }

      cout << "\r\n";
      i=0;
      typedef std::map<std::string, string>::iterator it_type1;

      for(it_type1 iterator = m_mapKeyToSignal.begin(); iterator != m_mapKeyToSignal.end(); iterator++)
      {
          sprintf(Text,"%1s=%12s: ",iterator->first.c_str(), iterator->second.c_str());
          cout << Text ;

          if( (i % 3) == 0  )
              cout << "\r\n";
      }
}

void ioimage::MakeSignal(std::string SignalName, std::string SignalMap)
{
    cout << "+ " << SignalName << "  "  << SignalMap << endl;

    IDigitalSignal *d;

    d = m_mapSignal[SignalName];
    if( d != NULL )
    {
        SignalNotFound.setReason("Signal wird bereits verwendet " + SignalName );
        throw SignalNotFound;
    }

    d = m_mapSignal[SignalMap];
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

    m_mapActiveSignal[SignalName] = d;
    return d;
}



