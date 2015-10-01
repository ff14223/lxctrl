#if 0
#include <global.h>
#include "VdsFrame.h"


#define FCMD_SEND_NORM        0
#define FCMD_SEND_NDAT        3


#define NDAT_STATUS				0x20
#define NDAT_INTERN				0x00
#define NDAT_BLOCKSTATUS		0x24
#define NDAT_UEGMSG     		0xB3
#define NDAT_SLOTINFORMTION		0xBB
#define NDAT_SYSTEMINTERNAL		0xBF


/*
**
*/
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



/*
** 
*/
void VdsParseUserFrame( TVDSUserFrame *pUserFrame, TVDSUserData *pUserData )
{
	/*
	** Decode Frame
	*/
	switch( pUserFrame->bType )
	{
		case NDAT_INTERN: 
			VdsParse_NDAT_INTERN( pUserFrame->bLen, pUserFrame->Data, pUserData);
			break;

		case NDAT_STATUS:
			VdsParse_NDAT_STATUS( pUserFrame->bLen, pUserFrame->Data, pUserData);
			break;

		case NDAT_SLOTINFORMTION:
			VdsParse_NDAT_SLOTINFORMTION( pUserFrame->bLen, pUserFrame->Data, pUserData);
			break;

		case NDAT_UEGMSG:
			VdsParse_NDAT_UEGMSG( pUserFrame->bLen, pUserFrame->Data, pUserData);
			break;

		case NDAT_SYSTEMINTERNAL:
			VdsParse_NDAT_SYSTEMINTERNAL( pUserFrame->bLen, pUserFrame->Data, pUserData);
			break;
			
		case NDAT_BLOCKSTATUS:
			/*LogWriteString((UDINT)"dType:NDAT_BLOCKSTATUS");*/
			break;
			
		case 0x56:
			VdsParse_NDAT_0x56( pUserFrame->bLen, pUserFrame->Data, pUserData);
			break;
	}
}


/* {group:VDS}
**
** Description: Empfangenen Frame von der ÜZ parsen.
**
** Remarks: Der Frame wird in die einzelnen Nutzerdaten zerteilt, und für jeden 
**      Nutzerdatenframe wird die Funktion VdsParseUserFrame(...) aufgerufen.
**      Für einen SEND_NORM Frame wird keine Parsen unterstützt.
**
** Arguments:
**     pFrame - Zeiger auf den Empfangenen Frame.
**     pUserData - Zeiger auf die UserDaten eines Frames.
**
** Returns: 
**     Nothing.
*/
void VdsParseFrame( TVDSFrame *pFrame, TVDSUserData *pUserData )
{
	char LogText[596];		/* Maximal 256 Zeichen in einem Frame -> in HEX + Header */
	int i;

	unsigned char offset = 0;
	unsigned char bFCode;
	unsigned char bFCB;
	unsigned char bFCV;
	TVDSUserFrame *pUserFrame;
	
	bFCode = pFrame->C & 0x0F;
	bFCV = (pFrame->C >> 4) & 0x01;
	bFCB = (pFrame->C >> 5) & 0x01;

	
	switch( bFCode )
	{
	case FCMD_SEND_NORM:
		/*
		** Send Norm braucht nicht weiter decodiert werden
		*/
		break;

	case FCMD_SEND_NDAT:
		memset( pUserData, 0, sizeof( TVDSUserData ) );

		/*
			Alle Frames werden einzeln durchgegangen. Da nicht bekannt ist, 
			wieviele Frames entahlten sind, wird solange eine durchgegangen 
			bis der offset > Framelänge
		*/
		while( offset < (pFrame->bLen-2) )
		{
			/*
				Frame start
			*/
			pUserFrame = (TVDSUserFrame *) &(pFrame->Data[offset]);
			
			/* 
				Auf nächsten Frame setzten
			*/
			offset+=pUserFrame->bLen + 2;			/* +2 Wegen Type und Länge */ 
			
			/*
				Die einzelnen Unterfunktionen zum auswerten des Frames
				führen Logbucheintragungen durch die zur anzeige gelangen 
				sollen. 
				
				Diese Frames sind mit einem A als ersten Buchstaben zu Kennzeichnen,
				der vom Windows Programm verworfen wird.
			*/
			VdsParseUserFrame( pUserFrame , pUserData);
		}
		
		/*
			Hier sind alle UserDaten die im Frame enthalten sind ausgewertet.
			
			Logbucheintrag mit dem empfangenen Frame.
			
			Diese Frames werden mit einem 'BIN::' gekennzeichnet, und in eine spezielle Datenbank gespeichert.
			
			Eintrag ins Logbuch durchführen
		*/


    	LogText[0] = 'B';
		LogText[1] = 'I';
		LogText[2] = 'N';
		LogText[3] = ':';
		LogText[4] = ':';
	
		/*
			String zusammenstellen
		*/
		/* Len */
		HexValue( pFrame->bLen-2, &(LogText[5]) );

		for( i=0; i<pFrame->bLen-2; i++)
		{
			 HexValue( pFrame->Data[i], &(LogText[i*2+7]) );
		}
		LogText[i*2+7] = 0;
		
		/*
			Und ins Logbuch eintragen
		*/
		LogWriteString( (UDINT) LogText );

		break;
	}
}


/* {group:VDS}
**
** Description: Empfängt einen VDS Frame. 
**
** Remarks: Für jedes Zeichen das von der seriellen Schnittstelle empfangen wird, 
**      wird diese Funktion aufgerufen, die einen gültigen Frame zusammenstellt.
**      Wird ein Frame empfangen wird die Funktion VdsParseFrame(...) aufgerufen.  
**      Danach wird VdsSendAck(...) aufgerufen um der ÜZ mitzuteilen, das der Frame
**      verarbeitet wurde. Die VDS Statemachine unterstütz nur das empfangen von Frames
**      d.h. einen Datenfluss von der ÜZ zur SPS.
**
** Arguments:
**     pVds - Zeiger auf die VDS Verwaltungs Struktur.
**     char Data - Datenbyte das empfangen wurde.
**
** Returns: 
**     Nothing.
*/
void VdsStateMachine(TVDS2465 *pVds, unsigned char Data)
{
	int Redo = 1;

	while( Redo == 1 )
	{
		Redo = 0;
		switch( pVds->uiState )
		{
		case VDS_FRAME_WAIT_FOR_START:				/* Warten auf Start (0x68)    */
			if( Data == 0x68 )
				pVds->uiState = VDS_FRAME_GET_LEN;
			break;

		case VDS_FRAME_GET_LEN:
			pVds->CurrFrame.bLen = Data;
			pVds->uiState = VDS_FRAME_VERIFY_LEN;
			break;

		
		case VDS_FRAME_VERIFY_LEN:
			if( pVds->CurrFrame.bLen == Data )
			{
				pVds->uiState = VDS_FRAME_GET_HEADER_END;
			}
			else
			{
				RESET_STATE_MACHINE(pVds);
				Redo = 1;
			}
			break;


		case VDS_FRAME_GET_HEADER_END:
			if( Data == 0x68 )
			{
				
				pVds->uiState = VDS_FRAME_GET_CFIELD;
			}
			else
			{
				RESET_STATE_MACHINE(pVds);
				Redo = 1;
			}
			break;


		case VDS_FRAME_GET_CFIELD:
			pVds->CurrFrame.C = Data;
			pVds->uiCheckSum = Data;				/* Checksumme ist die aritm. Summe uber C/A/Daten */
			pVds->uiState  = VDS_FRAME_GET_AFIELD;
			break;


		case VDS_FRAME_GET_AFIELD:
			pVds->CurrFrame.A = Data;
			pVds->uiCheckSum += Data;
			
			
			if( pVds->CurrFrame.bLen > 2 )
			{
				pVds->uiDataOffset = 0;
				pVds->uiState  = VDS_FRAME_GET_DATA;
			}
			else
			{
				pVds->uiState  = VDS_FRAME_GET_CHECKSUM;
			}

			break;


		case VDS_FRAME_GET_DATA:		/* Get Data */
			pVds->CurrFrame.Data[pVds->uiDataOffset++] = Data;
			pVds->uiCheckSum += Data;
			if( pVds->uiDataOffset >= (unsigned int)(pVds->CurrFrame.bLen - 2) )
			{
				pVds->uiState = VDS_FRAME_GET_CHECKSUM;
			}
			break;

		
		case VDS_FRAME_GET_CHECKSUM:		/* Get Checksum */
			if( Data == (pVds->uiCheckSum & 0xFF) )
			{
				pVds->uiState = VDS_FRAME_GET_FRAME_END;
			}
			else
			{
				RESET_STATE_MACHINE(pVds);
				Redo = 1;
			}
			break;


		case VDS_FRAME_GET_FRAME_END:
			if( Data == 0x16 )
			{
				/*
				** Ein Frame mehr empfangen
				*/
				pVds->uiRFrameCount++;
				VdsParseFrame( &(pVds->CurrFrame), &(pVds->UserData) );
				VdsSendAck( pVds );
			}
			else
			{
				Redo = 1;
			}
			
			/* Auf alle Fälle wieder von vorne */
			RESET_STATE_MACHINE(pVds);

			break;
		default:
			pVds->uiState  = VDS_FRAME_WAIT_FOR_START;
		}
	}
}



/* {group:VDS}
**
** Description: Sendet einen ACK Frame zur ÜZ7500.
**
** Remarks: Veresendet einen ACK Frame zur ÜZ7500. Wird nach jedem Empfangenen 
**     Frame aufgerufen. Setzt die interne Variable pVds->tNoAckSent wieder auf
**     0 wenn eine ACK gesendet wurde.
**
** Arguments:
**     pVds - Zeiger auf VDS Verwaltungs Struktur.
**
** Returns: 
**     Nothing.
*/
void VdsSendAck( TVDS2465 *pVds )
{
	unsigned char Ack[9] ={0x68,0x2,0x2,0x68,0x0,0x0,0x0,0x16};

	/*
	** Get a buffer 
	*/
	do {
		pVds->xSBuf.ident = pVds->SerDevice.ident;
	   	pVds->xSBuf.enable = 1;
    	FRM_gbuf( &(pVds->xSBuf) );
    	} while( pVds->xSBuf.status );
    	
    /*
    ** Data to write
    */
    pVds->xSBuf.buflng = 8;
    memcpy( (void*)pVds->xSBuf.buffer, Ack, pVds->xSBuf.buflng );
    
    pVds->FrmWrite.ident = pVds->SerDevice.ident;
    pVds->FrmWrite.buffer = pVds->xSBuf.buffer;
    pVds->FrmWrite.buflng = pVds->xSBuf.buflng; 
    pVds->FrmWrite.enable = 1;

	/*
	** Auftrag schreiben   
	*/
	FRM_write( &(pVds->FrmWrite) );
	
	/* 
	** im Fehlerfall muss Sendepuffer freigegeben werden 
	*/
	if( pVds->FrmWrite.status != 0)
	{
	   	pVds->FrmRobuf.ident =  pVds->SerDevice.ident;
        pVds->FrmRobuf.buffer = pVds->xSBuf.buffer;
        pVds->FrmRobuf.buflng = pVds->xSBuf.buflng;
        pVds->FrmRobuf.enable = 1 ;
		FRM_robuf( &(pVds->FrmRobuf) );
	}
	else
	{
		pVds->tNoAckSent = 0;
	}
}

#endif
