#include <iostream>
#include <src/settings.h>
#include <src/vds/vds.h>
#include <src/ioimage.h>
#include <src/test.h>
#include <syslog.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>

using namespace std;

void MakeSysLogEntry(char *Text)
{
    openlog("lxctrl", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, Text);
    closelog();
}

volatile bool bTerminate=false;
void sig_handler(int signum)
{
    printf("Received signal %d\n", signum);
    bTerminate = true;
}

int main()
{
    cout << "Linux Ctrl" << endl;
    MakeSysLogEntry("Starting lxCtrl...");
    signal(SIGINT, sig_handler);

    int fdBmaDevice=-1;
    int fdBmaLogFile=-1;

    try
    {
        /*
         * TESTING
        */
        Test *baseTest = new Test();
        baseTest->testVdsFrame();


        // INIT
        std::string bmaDeviceName = getSettings()->Cfg()->lookup("bma.device");
        std::string bmaLogFileName = getSettings()->Cfg()->lookup("bma.input-log");

        ioimage &io = getIOImage();
        IDigitalSignal *s = io.getSignal("inHausAlarm");

        vds *vds1 = new vds();
        // open serial device for BMZ Connection
        fdBmaDevice = open( bmaDeviceName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
        if( fdBmaDevice == -1 )
        {
            cout << "WARNING: No BMA Device set." << endl;
        }

        if( fdBmaDevice > 0)
        {
            struct termios options;

            tcgetattr(fdBmaDevice, &options);        // get current optionys

            cfsetispeed(&options, B9600);   // SET 9600 BAUD
            cfsetospeed(&options, B9600);

            options.c_cflag |= (CLOCAL | CREAD);
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS8;

            // options.c_cflag &= ~CN_RTSCTS;

            tcsetattr(fdBmaDevice, TCSANOW, &options);
        }
        fdBmaLogFile = open( bmaLogFileName.c_str() ,  O_RDWR | O_CREAT);
        if( fdBmaLogFile  < 0 )
            cout << "Could not open log..." << endl;
        unsigned char data[32];
        // other init
        data[0] = 'T';

        // Dmp BMA Data
        if( fdBmaLogFile  > 0 )
            write( fdBmaLogFile, data, 1 );


        cout << "Press 'Q' to quit..." << endl;
        // loop
        while( bTerminate == false )
        {
            sleep(1);


            // get char from serial port
            int nrBytesRead = read( fdBmaDevice, data, sizeof(data));
            if( nrBytesRead > 0)
            {

                cout << ":" << endl;

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

    cout << endl << "Closing File Descriptors."  << endl ;
    if( fdBmaDevice > 0 )
        close( fdBmaDevice );
    if( fdBmaLogFile > 0)
        close( fdBmaLogFile);

    return 0;
}

