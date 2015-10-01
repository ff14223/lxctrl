#include <iostream>
#include <src/settings.h>
#include <src/vds/vds.h>
#include <src/ioimage.h>
#include <src/test.h>
#include <syslog.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

using namespace std;

void MakeSysLogEntry(char *Text)
{
    openlog("lxctrl", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, Text);
    closelog();
}


int main()
{
    cout << "Linux Ctrl" << endl;
    MakeSysLogEntry("Starting lxCtrl...");

    try
    {
        /*
         * TESTING
        */
        Test *baseTest = new Test();
        baseTest->testVdsFrame();


        // INIT
        std::string bmaDeviceName = getSettings()->Cfg()->lookup("bma.device");
        std::string bmaLogFileName = getSettings()->Cfg()->lookup("bma.device");

        ioimage &io = getIOImage();
        IDigitalSignal *s = io.getSignal("inHausAlarm");

        vds *vds1 = new vds();
        // open serial device for BMZ Connection
        int fdBmaDevice = open( bmaDeviceName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
        if( fdBmaDevice == -1 )
        {

        }

        int fdBmaLogFile = open( bmaLogFileName.c_str() , O_APPEND );
        // other init

        unsigned char data[32];

        // loop
        while(1)
        {
            sleep(1);
            cout << "::::" << endl;

            // get char from serial port
            int nrBytesRead = read( fdBmaDevice, data, sizeof(data));
            if( nrBytesRead > 0)
            {
                // Dmp BMA Data
                if( fdBmaLogFile  > 0 )
                    write( fdBmaLogFile, data, nrBytesRead );

                for(int i=0;i<nrBytesRead;i++)
                    vds1->ReceiveFrameStateMachine( data[i] );
            }

            // get inputs

            // check for alarm

            // handle state machines
        }
    }
    catch( exception& e)
    {
        cout << endl << "Fehler beim starten."  << endl << e.what() << endl;
        MakeSysLogEntry("Fehler beim starten lxCtrl...");
        MakeSysLogEntry( (char*)e.what() );
    }

    return 0;
}

