#include "vds.h"
#include "iostream"
using namespace std;

vds::vds()
{
    pFrameReceive = new VdsFrame();
    enFrameReceiveState = VDS_FRAME_WAIT_FOR_START;
}

#define VDS_FRAME_START 0x68
#define VDS_FRAME_END   0x16

#define NDAT_STATUS				0x20
#define NDAT_INTERN				0x00
#define NDAT_BLOCKSTATUS		0x24
#define NDAT_UEGMSG     		0xB3
#define NDAT_SLOTINFORMTION		0xBB
#define NDAT_SYSTEMINTERNAL		0xBF

typedef struct
{
    unsigned short usKanal;		/* Kanal der Einsteckkarte   				*/
    unsigned short usAddresse;	/* Addresse der Einsteckkart 				*/
    unsigned int uiNumber;		/* Laufende Nummer der Meldung bei �Z 		*/
    unsigned short usReg;		/* wodurch wurde diese Meldung registriert 	*/
    unsigned int Art;			/*  */
    unsigned short usTransport; /* Transportdienst 							*/
    unsigned int uiID;			/*  */
}TVDSUserData;

void VdsParse_NDAT_SLOTINFORMTION(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
    switch( Data[1] )
    {
        case 0x38:
            /*uiErrInfo[ BMA_ERROR ] |= BAL_BMA_SCHICHT1_FEHER;
            bISDNBusStoerung = 1;*/
            /* LogWriteString((UDINT)"ASchicht 1 Fehler (ISDN S0)"); */
            break;

        case 0xB8:
            /*uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_SCHICHT1_FEHER;
            bISDNBusStoerung = 0;*/
            /* LogWriteString((UDINT)"ASchicht 1 ok (ISDN S0)"); */
            break;

        case 0xBF:
            /* LogWriteString((UDINT)"AGSM-Simkarte defekt"); */
            break;

        case 0xBE:
            /* LogWriteString((UDINT)"AGSM-Puknummer erforderlich"); */
            break;

        case 0xBD:
            /* LogWriteString((UDINT)"AGSM-Falsche Pinnummer"); */
            break;
    }
}

void VdsParse_NDAT_INTERN(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
#if 0
    char LogText[200];
    int i;

    LogText[0] = 'd';
    LogText[1] = 'x';
    LogText[2] = '0';
    LogText[3] = '0';
    LogText[4] = ':';

    for(i=0;i<bLen;i++)
    {
         HexValue( Data[i], &(LogText[i*2+5]) );
    }
    LogText[i*2+5] = 0;
    LogWriteString( (UDINT) LogText );
#endif
    cout << "VdsParse_NDAT_INTERN" << endl;
    pUserData->uiNumber = 0;/* GetBcdValue( Data[2] )*10000 + GetBcdValue( Data[3] )*100 + GetBcdValue( Data[4] );*/
    pUserData->usKanal     =  Data[12] >> 4;
    pUserData->usAddresse  =  Data[12] & 0x0F;
    pUserData->usTransport =  Data[13];
}

int cbFrame(int Len, unsigned char *pData, unsigned int cooky)
{

    unsigned char nType = pData[0];
    TVDSUserData data;

    /* TODO Move to vdsframe
     * cout << "Frame Dump " << endl;
    for(int i= 0; i < Len ; i++ )
    {
        cout << std::hex << (int)pData[i] << " ";
        if( i > 0 && (i%16)==0)
            cout << endl;
    }
    cout << endl; */

    cout << "Parsing UserFrame Type:" <<  std::hex << (int)nType << "  Len:" << Len << endl;

    switch( nType )
    {
        case NDAT_INTERN:
            VdsParse_NDAT_INTERN( Len, &pData[1], &data);
            break;

        /*case NDAT_STATUS:
            VdsParse_NDAT_STATUS( pUserFrame->bLen, pUserFrame->Data, pUserData);
            break;*/

        case NDAT_SLOTINFORMTION:
            VdsParse_NDAT_SLOTINFORMTION( Len, &pData[1], &data);
            break;

        /*case NDAT_UEGMSG:
            VdsParse_NDAT_UEGMSG( pUserFrame->bLen, pUserFrame->Data, pUserData);
            break;

        case NDAT_SYSTEMINTERNAL:
            VdsParse_NDAT_SYSTEMINTERNAL( pUserFrame->bLen, pUserFrame->Data, pUserData);
            break;

        case NDAT_BLOCKSTATUS:
            *LogWriteString((UDINT)"dType:NDAT_BLOCKSTATUS");
            break;

        case 0x56:
            VdsParse_NDAT_0x56( pUserFrame->bLen, pUserFrame->Data, pUserData);
            break;*/
    }

    return 0;
}



/* receive another char */
/*
 * VDS FRame
 * Start | LEN | LEN | HEADER | C | A | DATA | CRC
 * 0x68
 */
void vds::ReceiveFrameStateMachine(unsigned char Data)
{
    static unsigned char ucExpectedFrameLength;
    int Redo = 1;

    while( Redo == 1 )
    {
        Redo = 0;
        switch( enFrameReceiveState )
        {
        case VDS_FRAME_WAIT_FOR_START:				/* Warten auf Start (0x68)    */
            if( Data == 0x68  )
                enFrameReceiveState = VDS_FRAME_GET_LEN;
            break;

        case VDS_FRAME_GET_LEN:
            ucExpectedFrameLength = Data;
            enFrameReceiveState = VDS_FRAME_VERIFY_LEN;
            break;


        case VDS_FRAME_VERIFY_LEN:
            if( ucExpectedFrameLength == Data )
            {
                enFrameReceiveState = VDS_FRAME_GET_HEADER_END;
            }
            else
            {
                enFrameReceiveState = VDS_FRAME_WAIT_FOR_START;
                Redo = 1;   //  do not miss this char
            }
            break;


        case VDS_FRAME_GET_HEADER_END:
            if( Data == 0x68 )
            {
                enFrameReceiveState = VDS_FRAME_GET_CFIELD;
            }
            else
            {
                enFrameReceiveState = VDS_FRAME_WAIT_FOR_START;
                Redo = 1;
            }
            break;


        case VDS_FRAME_GET_CFIELD:
            pFrameReceive->SetC( Data );
            enFrameReceiveState  = VDS_FRAME_GET_AFIELD;
            break;


        case VDS_FRAME_GET_AFIELD:
            pFrameReceive->SetA( Data );
            if( ucExpectedFrameLength > 2 )
            {
                enFrameReceiveState  = VDS_FRAME_GET_DATA;
            }
            else
            {
                enFrameReceiveState  = VDS_FRAME_GET_CHECKSUM;
            }

            break;


        case VDS_FRAME_GET_DATA:		/* Get Data */
            pFrameReceive->AddData(Data);


            if( pFrameReceive->Length() >= (int)(ucExpectedFrameLength) )
            {
                enFrameReceiveState = VDS_FRAME_GET_CHECKSUM;
            }
            break;


        case VDS_FRAME_GET_CHECKSUM:		/* Get Checksum */
            if( Data != pFrameReceive->Checksumm() )
            {
                enFrameReceiveState = VDS_FRAME_GET_FRAME_END;
            }
            else
            {
                enFrameReceiveState = VDS_FRAME_WAIT_FOR_START;
                Redo = 1;
            }
            break;


        case VDS_FRAME_GET_FRAME_END:
            enFrameReceiveState = VDS_FRAME_WAIT_FOR_START;
            if( Data == 0x16 )
            {
                /*
                ** Ein Frame mehr empfangen
                */
                uiFramesReceived++;
                pFrameReceive->ForEachUserFrame(&cbFrame, 0 );
                SendAck( );
                pFrameReceive->Reset();
            }
            else
            {
                Redo = 1;
            }
            break;

        default:
            enFrameReceiveState  = VDS_FRAME_WAIT_FOR_START;
            break;
        }
    }
}


void vds::SendAck()
{
}

int vds::ParseUserFrame(unsigned char *pFrameStart)
{
    int FrameSize = *pFrameStart++;
    return FrameSize+2;
}

void HexValue(unsigned char Data, char *Text)
{
    int k;

    k = Data>>4;

    if( k > 9 )
    {
        Text[0]='A' + k - 10;
    }
    else
    {
        Text[0]='0' + k;
    }

    k = Data & 0x0F;

    if( k > 9 )
    {
        Text[1]='A' + k - 10;
    }
    else
    {
        Text[1]='0' + k;
    }
}

