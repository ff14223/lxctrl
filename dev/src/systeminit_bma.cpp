#include "inc/ISystemData.h"
#include <src/settings.h>
#include <src/ioimage.h>
#include <src/mysqladapter.h>
#include <src/alarm.h>
#include <src/alarmstatemachine.h>

#include <sys/file.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


#include <src/vds/vds.h>

class BmaConfigurationException : public exception
{
    string s1;
    virtual const char* what() const throw()
    {
        // s1.append("Einstellungen konnten nicht geladen werden");
        return s1.c_str();
    }
public:
    void setReason(string reason){s1=reason;}
    virtual ~BmaConfigurationException() throw() { }
} BmaConfig;


void signal_handler_IO(int status)
{
    getSystem()->BmaMain.bDataAvailable = true;
    getSystem()->BmaFailover.bDataAvailable = true;
}


int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
                return -1;

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays

        // Non blocking return immediately with data
           tty.c_cc[VMIN] = 0;
           tty.c_cc[VTIME] = 0;

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        // Control flags
           tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS
           tty.c_cflag |= CLOCAL; // Ignore status lines
           tty.c_cflag |= CREAD; // Enable receiver
           tty.c_cflag |= HUPCL; // Drop DTR on close

        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        // Local flags
        tty.c_lflag = 0;  // No local flags
        tty.c_lflag &= ~ICANON; // Don't canonicalise
        tty.c_lflag &= ~ECHO; // Don't echo
        tty.c_lflag &= ~ECHOK; // Don't echo


        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                return -1;

        // Clear the line
        tcflush(fd,TCIFLUSH);

        return 0;
}

int set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
                return -1;

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
             return -1;

        return 0;
}

int configure_device_as_bma_input(std::string bmaDeviceName)
{


    int fdBmaSerialPort = open (bmaDeviceName.c_str(), O_RDWR | O_NOCTTY |  O_NONBLOCK);
    if( fdBmaSerialPort < 0 )
    {
        cout << "WARNING: No BMA Device set. (" << bmaDeviceName << ")"<< endl;
        return -1;
    }



    fcntl(fdBmaSerialPort, F_SETOWN, getpid());          /* allow the process to receive SIGIO */
    fcntl(fdBmaSerialPort, F_SETFL, FASYNC);

    /* lock access so that another process can't also use the port */
    if(flock(fdBmaSerialPort, LOCK_EX | LOCK_NB) != 0)
    {
      close(fdBmaSerialPort);
      cout << "ERROR: Could not lock ComPort. (" << bmaDeviceName << ")"<< endl;
      return -1;
    }

    if( set_interface_attribs (fdBmaSerialPort, B9600, 0) < 0 )  // set speed to 115,200 bps, 8n1 (no parity)
    {
        close(fdBmaSerialPort);
        cout << "ERROR: Could not config ComPort. (" << bmaDeviceName << ")"<< endl;
        return -1;
    }

    set_blocking (fdBmaSerialPort, 0);

    return fdBmaSerialPort;
}

void init_main_bma(ISystem *pSystem)
{
    std::string bmaDeviceName = getSettings()->Cfg()->lookup("bma.device");
    pSystem->BmaMain.fd = configure_device_as_bma_input(bmaDeviceName );
    if( pSystem->BmaMain.fd  == -1 )
    {
        BmaConfig.setReason("Schnittstelle für Haupt BMA kann nicht geöffnet werden.");
        throw BmaConfig;
    }

    std::string bmaLogFileName = getSettings()->Cfg()->lookup("bma.input-log");
    pSystem->BmaMain.fdLog = open( bmaLogFileName.c_str() ,  O_RDWR | O_CREAT | O_APPEND);
    if( pSystem->BmaMain.fdLog < 0 )
    {
        cout << "WARNING: Could not open log..." << endl;
        BmaConfig.setReason("Input Log für Bma kann nicht geöffnet werden.");
        throw BmaConfig;
    }

    pSystem->BmaMain.Vds.pVdsInput = new vds( pSystem->Data.pIDb, &(pSystem->Signals), pSystem, pSystem->BmaMain.fd);
}

void init_failover_bma(ISystem *pSystem)
{
    std::string bmaDeviceName = getSettings()->Cfg()->lookup("bma1.device");
    pSystem->BmaFailover.fd = configure_device_as_bma_input(bmaDeviceName );
    pSystem->BmaFailover.Vds.pVdsInput = new vds( pSystem->Data.pIDb, &(pSystem->Signals), pSystem, pSystem->BmaFailover.fd);
}

void init_bma(ISystem *pSystem)
{
    struct sigaction saio={0};

    // install a signal handler
    saio.sa_handler = signal_handler_IO;
    sigaction(SIGIO,&saio,NULL);

    init_main_bma(pSystem);

    std::string runTest = getSettings()->Cfg()->lookup("bma.failsave-bma");
    if(  runTest.compare( "true" ) == 0 )
        init_failover_bma(pSystem);
}
