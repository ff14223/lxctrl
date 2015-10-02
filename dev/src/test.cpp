#include "test.h"
#include "exception"
#include "string"
#include <iostream>

#include "src/vdsframe.h"
#include "src/vds/vds.h"

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


void Test::TestFrame(vds *pVds)
{
    unsigned char Data[] ={ 0x68, 73, 73, 0x68, 0x73,0x03,
                            0x15,0x00,0x12,0x08,0x01,0xE4,0xC9,0x15,0x20,0x07,0x15,0x13,0x56,0x00,0x01,0xFF,0x06,0x66,0x46,0x01,0x65,0x35,0x69,0x06,0x52,0x66,0x46,0x01,0x65,0x35,0x69,
                            0x02,0xBB,0x20,0x12,
                            0x23,0x15,0x00,0x12,0x08,0x01,0xE4,0xC9,0x15,0x20,0x07,0x15,0x13,0x56,0x00,0x01,0xFF,0x06,0x66,0x46,0x01,0x65,0x35,0x69,
                            0x06,0x52,0x66,0x46,0x01,0x65,0x35,0x69,
                            0x02,0xBB,0x20,0x12, 
                            206, 0x16 };

    unsigned char Data1[] ={ 0x68, 0x17 ,0x17, 0x68, 0x73, 0x02,
    0x10, 0x00, 0x15, 0x08, 0x00, 0x03, 0xF3, 0x00, 0x20, 0x01, 0x03, 0x05, 0x14, 0x11, 0x12, 0xFF, 0x01, 0x5F,
    0x01, 0xB4, 0x89, 0x95, 0x16 };

    cout << "Size of Frame:" << sizeof(Data) << endl;

    for(int i=0; i<sizeof(Data);i++)
        pVds->ReceiveFrameStateMachine(Data[i]);


}

void Test::testVdsFrame(vds *pVds)
{
    SimpleFrame();
    TestFrame(pVds);
}


