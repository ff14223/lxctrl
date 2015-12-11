#include "test.h"
#include "exception"
#include "string"
#include <iostream>

#include "src/vdsframe.h"
#include "src/vds/vds.h"
#include <iostream>
#include <fstream>

using namespace std;

class AssertionFailedExeption: public exception
{
    string *s1;
    virtual const char* what() const throw()
    {
        // s1.append("Einstellungen konnten nicht geladen werden");
        return s1->c_str();
    }
public:
    void setReason(char *what){s1=new string(what);}
    virtual ~AssertionFailedExeption() throw() { }
} AssertionFailed;


Test::Test()
{
    cout << "Erstelle basis test...." << endl;
}

void Test::AssertEqual(int a, int b, char *what)
{
    if( a == b )
        return;

    AssertionFailed.setReason( what );
    throw AssertionFailed;
}


void Test::LoadSimulationFile(vds *pVds, const char *FileName)
{
    streampos size;
    char * memblock;
    ifstream myfile (FileName,ios::in | ios::binary);

    if( myfile.is_open()== false )
        return;

    unsigned char Buffer;

    myfile.seekg (0, myfile.beg);
    cout << "Simulating BMA Input" << endl;
    while( myfile.read((char*)&Buffer,1) )
    {
        cout << std::hex << (unsigned int)Buffer << " ";
        pVds->ReceiveFrameStateMachine(Buffer);
    }
    myfile.close();
}


void Test::SimpleFrame()
{
    cout << "Testing VdsFrame implementation" ;
    VdsFrame *pFrame = new VdsFrame();
    AssertEqual(pFrame->Checksumm(), 0, "Checksum after reset/ init wrong");
    AssertEqual(pFrame->Length(), 2, "Length after reset");
    pFrame->SetA(0x23);
    AssertEqual(pFrame->Checksumm(), 0x23, "Checksum");
    pFrame->Reset();
    AssertEqual(pFrame->Checksumm(), 0, "Checksum after reset/ init wrong");
    pFrame->AddData(0x99);
    AssertEqual(pFrame->Checksumm(), 0x99, "Checksum after reset/ init wrong");
    AssertEqual(pFrame->Length(), 3, "Invalid Lenth");
    cout << " OK" << endl;
}

void TestVdsFrameNorm(vds *pVds)
{
    unsigned char Data[] ={ 0x68, 0x02, 0x02, 0x68, 0x40, 0x02, 0x42, 0x16};

    for(int i=0; i<sizeof(Data);i++)
        pVds->ReceiveFrameStateMachine(Data[i]);
}

void TestRoutineFehlt(vds *pVds)
{
    unsigned char Data[] ={ 0x68, 0x02, 0x02, 0x68, 0x40, 0x02, 0x42, 0x16,
                            0x68, 0x1F, 0x1F, 0x68, 0x73, 0x02,
                            0x0F, 0x00, 0x11, 0x00, 0x00, 0x39, 0xD7, 0x99, 0x19, 0x07, 0x22, 0x06,
                            0x17, 0x50, 0x00, 0x00, 0x00,
                            0x06, 0x56, 0x21, 0x43, 0x65, 0x87, 0x09, 0x21,
                            0x02, 0xBF, 0x00, 0x0F,
                            0x93, 0x16 };

    cout << "VdsFrame - Routine fehlt" << endl;

    for(int i=0; i<sizeof(Data);i++)
        pVds->ReceiveFrameStateMachine(Data[i]);

}

void Test::TestFrame(vds *pVds)
{
    TestRoutineFehlt(pVds);
}

void Test::testVdsFrame(vds *pVds)
{
    SimpleFrame();
    TestFrame(pVds);
    //LoadSimulationFile(pVds,"/home/georg/work/lxctrl/dev/bma.bin");
}


