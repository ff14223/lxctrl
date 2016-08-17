#include "vds.h"
#include "iostream"
#include "stdio.h"

using namespace std;



vds::vds(IDatabase *pIDb, ISystemSignals *pSignals, ISystem *pSystem)
{
    this->pIDb = pIDb;                                  // database interface
    this->pSignals = pSignals;
    this->pSystem = pSystem;
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


void vds::VdsParse_NDAT_SLOTINFORMTION(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
    switch( Data[1] )
    {
        case 0x38:
            pSignals->bma.pISDNBusStoerung->set( true );
            pIDb->LogEntry( 1001, "ASchicht 1 Fehler (ISDN S0)");
            pSystem->Data.alarm.pStoerung->raise();
            break;

        case 0xB8:
            pSignals->bma.pISDNBusStoerung->set( false );
            pIDb->LogEntry( 1002, "ASchicht 1 ok (ISDN S0)");
            break;

        case 0xBF:
            pIDb->LogEntry( 1003, "Simmkarte defekt");
            break;

        case 0xBE:
            pIDb->LogEntry( 1004, "GSM PUK erforderlich");
            break;

        case 0xBD:
            pIDb->LogEntry( 1005, "GSM falsche Pin Nr");
            break;
        default:
            char Buffer[128];
            sprintf(Buffer,"VdsParse_NDAT_SLOTINFORMTION Unbekannter Frame 0x%x", Data[1]);
            pIDb->LogEntry( 1006, Buffer);
        break;

    }
}

void vds::VdsParse_NDAT_INTERN(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
    cout << "VdsParse_NDAT_INTERN" << endl;
    pUserData->uiNumber = 0;/* GetBcdValue( Data[2] )*10000 + GetBcdValue( Data[3] )*100 + GetBcdValue( Data[4] );*/
    pUserData->usKanal     =  Data[12] >> 4;
    pUserData->usAddresse  =  Data[12] & 0x0F;
    pUserData->usTransport =  Data[13];
}

void vds::VdsParse_NDAT_0x52(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
    // TelefonNr
}

void vds::VdsParse_NDAT_0x56(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
    /*
       Die nibbles sind geswapt. F�r ID:13 wird 31 in Data[2]
       gesendet.

       ACHTUNG: Wahrscheinlich ein Fehler in der �Z. Ist bei einem Update besonders zu
       beachten.

       Die ID wird nur f�r Anzeige verwendet. Ein St�rungsalarm wird bei jedem Eintrefen einer
       Routine von ID fehlt Nachricht ausgel�st. (VdsParse_NDAT_SYSTEMINTERNAL)
    */
    Data[2] = ((Data[2]<<4) & 0xF0) | ((Data[2]>>4) & 0x0F );
    Data[1] = ((Data[1]<<4) & 0xF0) | ((Data[1]>>4) & 0x0F );
    Data[0] = ((Data[0]<<4) & 0xF0) | ((Data[0]>>4) & 0x0F );

    /*
        Read the ID
    */
    pUserData->uiID = GetBcdValue( Data[0] )*10000 + GetBcdValue( Data[1] )*100 + GetBcdValue( Data[2] );

}


void vds::VdsParse_NDAT_UEGMSG(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
    cout << __FUNCTION__ << endl;

    if( Data[0] != 0x28 )
        return;

    if( Data[1] == 1 )
        return;

    /*
    ** ID
    */
    int id = GetBcdValue( Data[5] );
    id += GetBcdValue( Data[4] ) * 100;
    id += GetBcdValue( Data[3] ) * 10000;

    /*
    ** Status der Meldelinien
    */
    unsigned short wMLState = Data[8] | Data[9] << 8;

    /*
    ** Grund war Meldelinienänderung von Linie wMLChanged
    */
    unsigned char bMLChanged = Data[7] >> 4;
    unsigned char bStoerung = Data[7] & 0xF;

    if( Data[7] == 0 )
    {
        pSystem->Data.pIDb->LogEntry(1000, "Testmeldung BMZ Teilnehmer");
        return;
    }

    if( bMLChanged == 0 )    /* in diesem Fall ware es keine Alarmmeldung */
        return;

    //wMLMask = 1 << (wMLChanged-1);
    char bSignalText = Data[6];

    IBmzUser *pUser = pIDb->getBmzUser( id );
    if( pUser == NULL )
        pUser = pIDb->getBmzUser( 0 );

    IBmzUserStatus *pStatus = pIDb->getBmzUserStatus(pUser);
    if( pStatus == 0  )
    {
        // TODO Verhalten bei Alarm ID nicht vorhanden
        pSystem->Data.pIDb->LogEntry(1000, "BMZ Teilnehmer nicht vorhanden");
        pSystem->Data.alarm.pStoerung->raise();
    }
    else
    {
        pSystem->Data.pIDb->LogEntry(1001, "Alarm BMZ Teilnehemer");
        pSystem->Data.alarm.pStoerung->raise();
        // ToDo Set Status to Active
        // pStatus->
        pIDb->saveBmzUserStatus( pStatus );
    }


#if 0
    /*
    ** Mit allen Usern vergleichen
    */
    for(UserIndex = 0; UserIndex < BmaUsers.iCount; UserIndex++)
    {
        pUser = &(BmaUsers.BmaUser[UserIndex]);

        /*
        ** User ist disabled
        */
        if( pUser->Disabled )
        {
            /* LogWriteString((UDINT)"Disabled"); */
            continue;
        }

        /*
        ** Check ID
        */
        if( (pUser->wActivation & ID_MUST_MATCH) && (pUser->id != (unsigned int) id))
        {
            /* LogWriteString((UDINT)"ID must MATCH");	*/
            continue;
        }

        /* LogWriteString((UDINT)"Teilnehmer mit ID gefunden"); */


        /*
        ** Meldelinie
        */
        if( (pUser->wActivation & ML_MASK_MUST_MATCH) && ((pUser->wMLMask & wMLMask)==0) )
        {
            /* LogWriteString((UDINT)"ML Mask does not match");	 */
            continue;
        }

        /*
        ** Signal Text
        */
        if( pUser->wActivation & SIGNAL_TEXT_MUST_MATCH )
        {
            if( pUser->SignalText != bSignalText )
                continue;
        }

        /* LogWriteString((UDINT)"BMA Alarm ausl�sen.."); */
        VdsActivateBMZTeilnehmer(pUser);
    }
#endif

}

void vds::VdsParse_NDAT_SYSTEMINTERNAL(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
    IBmzUser *pUser;
    char Text[255];

    cout << __FUNCTION__ << endl;
    switch( Data[1] )
    {
        case 0x00:
            pIDb->LogEntry( 202, "?");
            break;

        case 0x0F: /* Routine von Teilnehmer fehlt */
            /*
            ** Auf alle F�lle merken
            */

            pUser = pIDb->getBmzUser( pUserData->uiID );
            if( pUser == 0)
                pUser = pIDb->getBmzUser( 0 );

            if( pUser )
            {
                IBmzUserStatus * pStatus = pIDb->getBmzUserStatus( pUser );
                if( pStatus )
                {
                    int count = pStatus->getRoutineMissingCount();
                    if( count<= 0 )
                    {
                        // erstes Auftreten
                        snprintf(Text, 255, "Routine von Teilnehmer \"%s\" fehlt", pUser->getName().c_str() );
                        pIDb->LogEntry( 203, Text);
                    }
                    pStatus->setRoutineMissingCount(++count);
                    pIDb->saveBmzUserStatus( pStatus );
                    delete pStatus;
                }
                delete pUser;
            }
            else
            {
                pIDb->LogEntry( 203, "Routine von Teilnehmer fehlt.");
                pIDb->LogEntry( 203, "Unbekannte ID.");
            }
            pSystem->Data.alarm.pRoutineMissing->raise();
            pSystem->Data.alarm.pStoerung->raise();
            break;

        case 0x01:
            pIDb->LogEntry( 231, "Akku Fehler");
            pSignals->warning.pBmaAkkuFehler->set( true );
            pSystem->Data.alarm.pStoerung->raise();
            break;

        case 0x02:
            pIDb->LogEntry( 232, "Akku OK");
            pSignals->warning.pBmaAkkuFehler->set( false );
            break;

        case 0x03:
            pIDb->LogEntry( 233, "Netz Fehler");
            pSignals->warning.pBmaNetzFehler->set( true );
            pSystem->Data.alarm.pStoerung->raise();
            break;

        case 0x04:
            pIDb->LogEntry( 234, "Netz OK");
            pSignals->warning.pBmaNetzFehler->set( false );
            break;

        case 0x0B:
            pIDb->LogEntry( 235, "COM1 Offline");
            pSignals->warning.pBmzCom1Offline->set(true);
            pSystem->Data.alarm.pStoerung->raise();
            break;

        case 0x0C:
            pIDb->LogEntry( 236, "COM1 Online");
            pSignals->warning.pBmzCom1Offline->set(false);
            break;

        default:
            sprintf(Text,"%s %d Missing Case %d",__FUNCTION__,__LINE__,Data[1] );
            pIDb->LogEntry( 1002, Text);
            break;

    }
}


int vds::cbFrame(int Len, unsigned char *pData, unsigned int cooky)
{

    unsigned char nType = pData[0];


    switch( nType )
    {
        case NDAT_INTERN:
            VdsParse_NDAT_INTERN( Len, &pData[1], &m_data);
            break;

        /*case NDAT_STATUS:
            VdsParse_NDAT_STATUS( pUserFrame->bLen, pUserFrame->Data, pUserData);
            break;*/

        case NDAT_SLOTINFORMTION:
            VdsParse_NDAT_SLOTINFORMTION( Len, &pData[1], &m_data);
            break;

        case NDAT_UEGMSG:
            VdsParse_NDAT_UEGMSG( Len, &pData[1], &m_data);
            break;

        case NDAT_SYSTEMINTERNAL:
            VdsParse_NDAT_SYSTEMINTERNAL( Len, &pData[1], &m_data);
            break;

        case 0x52:
            // wenn auch ein Frame mit 0xBB enthaltenist, so ist die Tel.Nr codiert
            break;

        case 0x56:
            VdsParse_NDAT_0x56( Len, &pData[1], &m_data);
            break;

        default:
            cout << "?: " <<  std::hex << " " << (int)nType << "  Len:" << Len << endl;
            char Text[200];
            sprintf(Text,"unbehandelter Frame Type:0x%02x Länge:%d", nType, Len);
            pIDb->LogEntry( 10100, Text );
        break;
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
    static unsigned int uiCheckSum;

    while( Redo == 1 )
    {
        Redo = 0;

        switch( enFrameReceiveState )
        {
        case VDS_FRAME_WAIT_FOR_START:				/* Warten auf Start (0x68)    */
            if( Data == 0x68  )
            {
                enFrameReceiveState = VDS_FRAME_GET_LEN;
                pFrameReceive->Reset();
            }
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
                pSystem->Counter.VdsFramesErrors++;
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
                pSystem->Counter.VdsFramesErrors++;
                Redo = 1;
            }
            break;


        case VDS_FRAME_GET_CFIELD:
            uiCheckSum = Data;
            pFrameReceive->SetC( Data );
            enFrameReceiveState  = VDS_FRAME_GET_AFIELD;
            break;


        case VDS_FRAME_GET_AFIELD:
            uiCheckSum += Data;
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
            uiCheckSum += Data;
            /*cout << " Offset:" << pFrameReceive->Length()
                 << " uiCheckSum:" << uiCheckSum
                 << " pFrameReceive->Checksumm()" << (int)pFrameReceive->Checksumm() << endl;*/
            /* if( Data == uiCheckSum )
                cout << "Date == Checksum "; */

            if( pFrameReceive->Length() >= (int)(ucExpectedFrameLength) )
            {
                enFrameReceiveState = VDS_FRAME_GET_CHECKSUM;
            }
            break;


        case VDS_FRAME_GET_CHECKSUM:		/* Get Checksum */
            if( Data == pFrameReceive->Checksumm() )
            {
                enFrameReceiveState = VDS_FRAME_GET_FRAME_END;
            }
            else
            {
                cout << "Invalid Checksum  Data was:0x" << std::hex << (int) Data << "CSum:" << (int) pFrameReceive->Checksumm()  << endl;
                enFrameReceiveState = VDS_FRAME_WAIT_FOR_START;
                enFrameReceiveState = VDS_FRAME_GET_FRAME_END;
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
                pSystem->Counter.VdsFramesReceived++;
                int offset=0;
                unsigned char Data[258];
                int count;

                // Clear User Data
                m_data.uiID=0;
                while( (count=pFrameReceive->GetNDatFrame(&offset, Data)) > 0 )
                {
                    /*for(int j=0; j<count;j++)
                        cout << std::hex << (int) Data[j] << " "; */
                    /* cout << endl; */
                    cbFrame(count, Data, 0);
                }
                //pFrameReceive->ForEachUserFrame(&vds::cbFrame, 0 );
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

unsigned int GetBcdValue( unsigned char bcd )
{
    return( (unsigned int) ((bcd>>4)*10 + (bcd & 0x0F)) );
}
