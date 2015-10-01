#include "vdsframe.h"

#include <iostream>

using namespace std;

void VdsFrame::AddData(unsigned char data)
{
    m_Data[OFFSET_LENGTH]++;
    *pData = data;                  // neue daten
    pData++;                        // nächste position
}

VdsFrame::VdsFrame()
{
    Reset();
}

void VdsFrame::Reset()
{
    m_Data[1] = m_Data[0] = 0x68;
    m_Data[OFFSET_LENGTH] = 2;
    m_Data[OFFSET_ADDR] = 1;
    m_Data[OFFSET_STEUER] =0;
    m_Data[OFFSET_ADDR]=0;
    pData = &m_Data[OFFSET_DATA];
}

void VdsFrame::SetC(unsigned char c)
{
    m_Data[OFFSET_STEUER] = c;
}

void VdsFrame::SetA(unsigned char a)
{
    m_Data[OFFSET_ADDR] = a;
}

int VdsFrame::Length(){return (int) m_Data[OFFSET_LENGTH];}

#define FCMD_SEND_NORM			0
#define FCMD_SEND_NDAT			3

void VdsFrame::ForEachUserFrame( cbUserFrame cb, unsigned int cbCooky )
{
    unsigned int offset = 0;
    unsigned char bFCode = getC() & 0x0F;

    unsigned char *p = (unsigned char*)&(m_Data[OFFSET_DATA]);

    switch( bFCode )
    {
    case FCMD_SEND_NORM:
        /*
        ** Send Norm braucht nicht weiter decodiert werden
        */
        break;

    case FCMD_SEND_NDAT:
        //memset( pUserData, 0, sizeof( TVDSUserData ) );

        /*
            Alle Frames werden einzeln durchgegangen. Da nicht bekannt ist,
            wieviele Frames entahlten sind, wird solange eine durchgegangen
            bis der offset > Framel�nge
        */
        while( offset <= (Length()) )
        {
            cout << "Offset:" << offset;
            cb( p[offset], &p[offset+1], 0);
            offset += p[offset] + 2;
        }
        break;
    }

}

unsigned char VdsFrame::Checksumm()
{
    unsigned char ucSum=0;
    int l = m_Data[OFFSET_LENGTH];
    for(int i=0;i<l;i++)
    {
        ucSum += m_Data[OFFSET_STEUER+i];
    }
    return ucSum;
}
