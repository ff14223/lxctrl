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
#include <src/mysqladapter.h>


#include <time.h>

#include <inc/ISystemData.h>
ISystemData systemData={0};             // global data storage
ISystemSignals systemSignals={0};

void init(ISystemData *pSystemData);
void init_signals(ISystemData *pSystemData, ISystemSignals *pSignals);
using namespace std;

void MakeSysLogEntry(const char *Text)
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

void ctrl_general(ISystemData *pSystemData, ISystemSignals *pSignals);
void ctrl_alarm(ISystemData *pSystemData, ISystemSignals *pSignals);

int main()
{

    cout << "Linux Ctrl" << endl;

    signal(SIGINT, sig_handler);

    int fdBmaDevice=-1;
    int fdBmaLogFile=-1;

    try
    {
        // INIT
        init( &systemData );
        init_signals(&systemData, &systemSignals);
        // Database
        /*std::string dbUser = getSettings()->Cfg()->lookup("db.user");
        std::string dbPwd = getSettings()->Cfg()->lookup("db.pwd");
        IDatabase *db = new MySqlAdapter( dbUser.c_str(), dbPwd.c_str() ); */


        std::string bmaDeviceName = getSettings()->Cfg()->lookup("bma.device");
        std::string bmaLogFileName = getSettings()->Cfg()->lookup("bma.input-log");

        vds *vds1 = new vds( systemData.pIDb, &systemSignals );

        // open serial device for BMZ Connection
        fdBmaDevice = open( bmaDeviceName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
        if( fdBmaDevice == -1 )
            cout << "WARNING: No BMA Device set. (" << bmaDeviceName << ")"<< endl;

        if( fdBmaDevice > 0)
        {
            cout << "Configuring serial device for 8N1 9600" << endl;
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
            cout << "WARNING: Could not open log..." << endl;

        unsigned char data[32];

        // enter production loop
        cout << "Press CTRL-C to quit..." << endl;
        systemData.pIDb->LogEntry( 1000, "lxctrl is up and running" );


        /*
         * Timing
        */
        int milliseconds = 1000;
        struct timespec ts;

        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;

        /*
         * TESTING
        */
        std::string runTest = getSettings()->Cfg()->lookup("lxctrl.runtest");
        if(  runTest.compare( "true" ) == 0 )
        {
            Test *baseTest = new Test();

            while( bTerminate == false )
            {
                nanosleep(&ts, NULL);
                baseTest->testVdsFrame( vds1 );
                break;
            }

            return(1);
        }


        MakeSysLogEntry("lxctrl is up and runnuing");

        milliseconds=250;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;


        while( bTerminate == false )
        {
            nanosleep(&ts, NULL);

            systemData.pIo->UpdateInputs();
            
            // get char from serial port
            int nrBytesRead = read( fdBmaDevice, data, sizeof(data));
            if( nrBytesRead > 0)
            {

                cout << ":" ;

                // Dmp BMA Data
                if( fdBmaLogFile  > 0 )
                    write( fdBmaLogFile, data, nrBytesRead );

                for(int i=0;i<nrBytesRead;i++)
                    vds1->ReceiveFrameStateMachine( data[i] );
            }

            // allgemeine Dinge
            ctrl_general(&systemData, &systemSignals);

            // check for alarm
            ctrl_alarm(&systemData, &systemSignals);

            // handle state machines
            systemData.pIo->UpdateOutputs();
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

