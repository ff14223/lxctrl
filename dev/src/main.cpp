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
#include <string.h>

// globale daten
ISystem System;


/* Init functions are only callable from here */
void init(ISystem *pSystem);
void init_signals(ISystemData *pSystemData, ISystemSignals *pSignals);
void init_alarms(ISystem *pSystem);
void RunTestsIfEnabled(ISystem *pSystem);


void daemon_init();
void MakeSysLogEntry(const char *Text);

using namespace std;

void init_bma(ISystem *pSystem);

volatile bool bTerminate=false;
void sig_handler(int signum)
{
    printf("Received signal %d\n", signum);
    bTerminate = true;
}

void ctrl_general(ISystem*pSystem);
void ctrl_alarm(ISystem *pSystem);
void ctrl_bma(ISystem *pSystem);
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

ISystem* getSystem() { return &System; }

void Start()
{
    int hr =  pthread_attr_init( &(MainThread.attrib) );
    hr=pthread_attr_setinheritsched(&(MainThread.attrib), PTHREAD_EXPLICIT_SCHED);

    MainThread.sched_param.__sched_priority = 20;

    if( pthread_create ( &(MainThread.id), NULL, &lxctrl_main, NULL) != 0 )
    {
             cout <<  "Fehler beim erstellen der Threads\n" << endl;
             exit (EXIT_FAILURE);
    }

    hr = pthread_setschedparam( MainThread.id, SCHED_RR, &(MainThread.sched_param) );
}


bool bDisplayStatus = false;

int main(int argc, char *argv[])
{
    for(int i=1; i<argc; i++)
    {
        if( strcmp(argv[i], "--status") == 0)
            bDisplayStatus=true;
        else
        {
            cout << "Invalid Option " << argv[i] << endl;
            exit(1);
        }
    }

    //Set our Logging Mask and open the Log
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog("lxctrld", LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

    if( bDisplayStatus == false )
        daemon_init();

    //Start();
    lxctrl_main( argv );

    int *ret;
    pthread_join (MainThread.id, (void**) &ret);

    closelog();
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
#include <sys/file.h>



void* lxctrl_main(void*)
{

    cout << "Linux Ctrl" << endl;

    signal(SIGINT, sig_handler);

    int fdBmaLogFile =-1;
    try
    {

        // ----------------------------------------------------
        // INIT
        // ----------------------------------------------------
        init( &System );
        init_signals( &(System.Data), &(System.Signals));
        init_alarms( &System );
        init_bma(&System);

        if( System.Data.pIo->getNrSimulationMappings() > 0 )
            set_conio_terminal_mode();




        /* install the signal handler before making the device asynchronous */




        // ----------------------------------------------------
        // Testing stuff
        // ----------------------------------------------------
        RunTestsIfEnabled( getSystem() );


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

        //printf("\e[?25l");  //Cursor off
        struct timespec requestStart, requestEnd;

        clock_gettime(CLOCK_REALTIME, &requestEnd); /* init value to now */



        //
        // alte nachrichten abholen
        //

        while( bTerminate == false )
        {

            nanosleep(&ts, NULL);           /* wait */
            System.Counter.MainLoops++;

            /*
             * get time elapsed
             */
            clock_gettime(CLOCK_REALTIME, &requestStart);
            unsigned long t =  ( requestStart.tv_sec - requestEnd.tv_sec ) * 1000
                    + ( requestStart.tv_nsec - requestEnd.tv_nsec ) / 1000 / 1000;
            if( t > 1000000)
                t = 1000000;

            System.Values.tSleep = t;

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



            // BMA
            ctrl_bma( &System );

            // allgemeine Dinge
            ctrl_general( &System);

            // check for alarm
            ctrl_alarm( &(System));

            // handle state machines
            System.Data.pIo->UpdateOutputs();



            // This realy takes time
            // Show
            //printpage(&System);


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
    if( System.BmaMain.fd > 0 )
        close(System.BmaMain.fd);

    return 0;
}

void RunTestsIfEnabled(ISystem *pSystem)
{
    vds *pVds = new vds( pSystem->Data.pIDb, &(pSystem->Signals), pSystem, -1);
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
