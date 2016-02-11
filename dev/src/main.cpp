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
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <inc/ISystemData.h>
#include <stdio.h>


ISystem System={0};

/* Init functions are only callable from here */
void init(ISystem *pSystem);
void init_signals(ISystemData *pSystemData, ISystemSignals *pSignals);
void init_alarms(ISystem *pSystem);
void RunTestsIfEnabled(vds *pVds);

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

void ctrl_general(ISystem*pSystem);
void ctrl_alarm(ISystemData *pSystemData, ISystemSignals *pSignals);
void printpage(ISystem *pSystem);

#define MAX_THREADS 1

static int ret[MAX_THREADS];
static pthread_t th[MAX_THREADS];


typedef struct
{
    pthread_attr_t attrib;
    struct sched_param sched_param;
    pthread_t id;
}LxThreadInfo;

static LxThreadInfo MainThread;
void* lxctrl_main(void*);


void Start()
{
    int hr =  pthread_attr_init( &(MainThread.attrib) );
    hr=pthread_attr_setinheritsched(&(MainThread.attrib), PTHREAD_EXPLICIT_SCHED);

    MainThread.sched_param.__sched_priority = 20;

    if( pthread_create ( &(MainThread.id), NULL, &lxctrl_main, NULL) != 0 )
    {
             cout <<  "Fehler beim erstellen der Threads\n";
             exit (EXIT_FAILURE);
    }

    hr = pthread_setschedparam( MainThread.id, SCHED_RR, &(MainThread.sched_param) );
}
#include<QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication coreApplication(argc, argv);

    Start();

    int *ret;
    pthread_join (MainThread.id, (void**) &ret);
    return 0;
}

#include <sys/select.h>
#include <termios.h>
#include <memory.h>

struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

#include <stdio.h>
#include <errno.h>
#include <QtSerialPort/QtSerialPort>

void* lxctrl_main(void*)
{

    QSerialPort *serialport;
    cout << "Linux Ctrl" << endl;

    signal(SIGINT, sig_handler);

    int fdBmaLogFile=-1;

    try
    {

        // ----------------------------------------------------
        // INIT
        // ----------------------------------------------------
        init( &System );
        init_signals( &(System.Data), &(System.Signals));
        init_alarms( &System );

        serialport = new QSerialPort();
        serialport->setBaudRate( QSerialPort::Baud1200 );
        serialport->setDataBits( QSerialPort::Data8 );
        serialport->setStopBits( QSerialPort::OneStop );
        serialport->setFlowControl( QSerialPort::NoFlowControl );

        if( System.Data.pIo->getNrSimulationMappings() > 0 )
            set_conio_terminal_mode();


        std::string bmaDeviceName = getSettings()->Cfg()->lookup("bma.device");
        std::string bmaLogFileName = getSettings()->Cfg()->lookup("bma.input-log");

        vds *vds1 = new vds( System.Data.pIDb, &(System.Signals), &System);

        serialport->setPortName( QString::fromStdString( bmaDeviceName ) );
        if( serialport->open( QIODevice::ReadWrite) == false )
        {
            cout << "WARNING: No BMA Device set. (" << bmaDeviceName << ")"<< endl;
        }

        fdBmaLogFile = open( bmaLogFileName.c_str() ,  O_RDWR | O_CREAT);
        if( fdBmaLogFile  < 0 )
        {
            cout << "WARNING: Could not open log..." << endl;
            printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
            bTerminate = true;
        }


        // ----------------------------------------------------
        // Testing stuff
        // ----------------------------------------------------
        RunTestsIfEnabled(vds1);


        // ----------------------------------------------------
        // enter production loop
        // ----------------------------------------------------
        cout << "Press CTRL-C to quit..." << endl;
        System.Data.pIDb->LogEntry( 1000, "lxctrl is up and running" );



        MakeSysLogEntry("lxctrl is up and runnuing");

        int milliseconds = 250;
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        char data[128];

        //printf("\e[?25l");  //Cursor off
        struct timespec requestStart, requestEnd;

        clock_gettime(CLOCK_REALTIME, &requestEnd); /* init value to now */

        while( bTerminate == false )
        {
            /* wait */
            nanosleep(&ts, NULL);
            System.Counter.MainLoops++;

            /*
             * get time elapsed
             */
            clock_gettime(CLOCK_REALTIME, &requestStart);
            System.Values.tSleep = ( requestStart.tv_sec - requestEnd.tv_sec ) * 1000
              + ( requestStart.tv_nsec - requestEnd.tv_nsec ) / 1000 / 1000;

            System.Data.pIo->UpdateInputs();


            /*
             * simulate inputs, if q is pressed programm will terminate */

            if( System.Data.pIo->getNrSimulationMappings() > 0 && kbhit() )
            {
                char c = getch();
                System.Data.pIo->KeyPressed( c );
                if( c == 'q' )
                    bTerminate = true;
            }

            if( serialport->bytesAvailable() > 0 )
            {
                int nrBytesRead  = serialport->read(data,sizeof(data) );
                if( nrBytesRead > 0)
                {
                    // Dmp BMA Data
                    if( fdBmaLogFile  > 0 )
                        write( fdBmaLogFile, data, nrBytesRead );

                    for(int i=0;i<nrBytesRead;i++)
                    {
                        vds1->ReceiveFrameStateMachine( data[i] );
                        System.Counter.BmzBytesReceived++;
                    }
                }
            }

            // allgemeine Dinge
            ctrl_general( &System);

            // check for alarm
            ctrl_alarm( &(System.Data), &(System.Signals));

            // handle state machines
            System.Data.pIo->UpdateOutputs();



            // This realy takes time
            // Show
            printpage(&System);


            clock_gettime(CLOCK_REALTIME, &requestEnd);
            System.Values.tloop = ( requestEnd.tv_sec - requestStart.tv_sec ) * 1000
              + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / 1000 / 1000;



        }
    }
    catch( exception& e)
    {
        cout << endl << "Fehler beim starten."  << endl << e.what() << endl;
        MakeSysLogEntry("Fehler beim starten lxCtrl...");
        MakeSysLogEntry( (char*)e.what() );
    }

    printf("\e[?25h");
    cout << endl << "Closing File Descriptors."  << endl ;

    serialport->close();

    if( fdBmaLogFile > 0)
        close( fdBmaLogFile);

    return 0;
}

void RunTestsIfEnabled(vds *pVds)
{
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
            baseTest->testVdsFrame( pVds );
            break;
        }
    }
}
