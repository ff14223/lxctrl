#include "vdsframe.h"

#include <iostream>

using namespace std;

#define OFFSET_LENGTH   1
#define OFFSET_STEUER   3
#define OFFSET_ADDR     4
#define OFFSET_DATA     5
#define OFFSET_CHECKSUM 6

unsigned char VdsFrame::getC(){ return m_Data[OFFSET_STEUER]; }
unsigned char VdsFrame::getA(){ return m_Data[OFFSET_ADDR];}

void VdsFrame::AddData(unsigned char data)
{
    m_Data[OFFSET_LENGTH]++;
    *pData = data;                  // neue daten
    pData++;                        // nächste position
    uiCheckSum += data;
}

VdsFrame::VdsFrame()
{
    Reset();
}

void VdsFrame::Reset()
{
    uiCheckSum = 0;
    m_Data[2] = m_Data[0] = 0x68;
    m_Data[OFFSET_LENGTH] = 2;
    m_Data[OFFSET_ADDR] = 1;
    m_Data[OFFSET_STEUER] =0;
    m_Data[OFFSET_ADDR]=0;
    pData = &m_Data[OFFSET_DATA];
}

void VdsFrame::SetC(unsigned char c)
{
    m_Data[OFFSET_STEUER] = c;
    uiCheckSum += c;
}

void VdsFrame::SetA(unsigned char a)
{
    m_Data[OFFSET_ADDR] = a;
    uiCheckSum += a;
}

int VdsFrame::Length(){return (int) m_Data[OFFSET_LENGTH];}

#define FCMD_SEND_NORM			0
#define FCMD_SEND_NDAT			3

#include <memory.h>


void VdsFrame::Dump()
{
    int iSize =  m_Data[OFFSET_LENGTH];
    unsigned char *p = (unsigned char*)&(m_Data[OFFSET_DATA]);
    for(int i=0;i<iSize;i++)
    {
        cout << std::hex << (int)m_Data[i + OFFSET_DATA] << " ";
    }
}

int VdsFrame::GetNDatFrame(int *pOffset, unsigned char *pData)
{
    unsigned int offset = *pOffset;
    unsigned char bFCode = getC() & 0x0F;

    unsigned char *p = (unsigned char*)&(m_Data[OFFSET_DATA]);

    if( bFCode != FCMD_SEND_NDAT)
        return -1;


    if( offset < (Length()-2) )
    {
        int count = p[offset]+1;
        memcpy( pData, &p[offset+1], count ); // copy to data +1 weil art nicht in size
        //cout << "    Offset:" << offset<< " Count:" << count << " Type:" <<  (int)(pData[0]) << endl;
        *pOffset += count+1;
        return count;
    }

    return -1;
}

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
            //cout << "Offset:" << offset;
            cb( p[offset], &p[offset+1], 0);
            offset += p[offset] + 2;
        }
        break;
    }
}

unsigned char VdsFrame::Checksumm()
{
    return (unsigned char) (uiCheckSum & 0xFF);
}
