#if 0
#include "global.h"
#include "logger.h"
#include "BMAUsers.h"


#warning ISDN Verbindung prüfen.
#warning CRC Prüfsumme über BMZ Teilnehmer.
#warning Logbucheintragungen allgemein.


_LOCAL USINT dgb1;
_LOCAL unsigned int dgbid;

/*
** Sendebuffer
*/
_LOCAL FRM_gbuf_typ   xSBuf;
_LOCAL FRM_robuf_typ  FrmRobuf;
_LOCAL FRM_write_typ  FrmWrite;

unsigned int GetBcdValue( unsigned char bcd );
void VdsSendAck(FRM_xopen_typ *pDevice);
BOOLEAN VdsNDatFrame(FRM_xopen_typ *pDevice, VDSFrame *pFrame);


BOOLEAN HandleVdsFrame(FRM_xopen_typ *pDevice, VDSFrame *pFrame)
{
	BOOLEAN ret = FALSE;
	
	/*FSC.FramesReceived.iFrames++;*/						/* Alle Frames werden gezählt */
	
	if( !(pFrame->Cmd & CMD_FLAGS_DIRECTION_OUT ))
	{
		/*
		** Frame kommt zu mir 
		*/
		switch( pFrame->Cmd & CMD_MASK )
		{
		case CMD_SEND_NDAT:
			VDS2465.FramesReceived.uiNDat++;
			VDS2465.tNDat = 0;
			ret = VdsNDatFrame(pDevice, pFrame);
			/*VdsSendAck( pDevice );*/
			break;
			
		case CMD_SEND_NORM:
			VDS2465.FramesReceived.uiSendNorm++;
			uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_COMMUNICATION;
			if( VDS2465.tSendNorm > 200 )
			{
				/*ERR_warning( 0x0010, "BMA::Verbindung hergestellt");*/
			}
			VDS2465.tSendNorm = 0;
			VdsSendAck( pDevice ); 		/* send ack on devcie */
			ret = TRUE;
			break;
		}
	}
	
	return( ret );
}


BOOLEAN VdsDecodeSystemInternalFrame(VDSFrame *pFrame)
{
	unsigned char *p;
	int len = pFrame->Data[0];
	int i; 
	unsigned char bData;
	unsigned long offset;
	unsigned int id=0xFFFFFFFF;
	/*
	** Nur 0xBF / 0x56 
	*/
	p = pFrame->Data;


	/*
	** Nutzerdatensatz 0x00 überspringen. Länge des 
	** Datensatzes steht in pFrame->Data[0]
	*/ 
	p = &(pFrame->Data[	len + 2 ]); /* +2 Wegen Size und Types */

	/*
	** Wenn Satz Type = 0x56 und länge = 6
	** id auswerten
	*/
	if( p[1] == 0x56 && p[0] == 0x06 )
	{
		
		/* ID steht in p[2] bis p[7] leider sind die BCD Values
		** vertauscht. daher werden hier alle Nibble in diesen 
		** Bytes getauscht 
		*/
		       /* 0000000000 */
        offset = 1000000;
		id = 0;
		for(i=3;i<8;i++)
		{
			bData  =  (p[i]<<4) & 0xF0;
			bData |= (p[i]>>4) & 0x0F;

			id += offset * (unsigned long) GetBcdValue( bData );
			offset /= 10;
		}

		/*
		** Frame start auf nächsten Frame
		*/ 
		p = &(p[p[0]+2]);
	}
	dgbid = id;
	dgb1=2;
	if( p[1] != 0xBF )    /* Satztype */
		return FALSE;     /* Nur 0xBB Typen ( Slotinformation ) */

	dgb1 = 3;

	
	switch( p[2] )   /* Kennung */
	{
	case 0x00: /* ohne Bediencode */
		break;

	case 0x01: /* mit Bediencode */
		break;

	case 0x02:
		break; /* mi Slotangabe */
	}
	dgb1 = 4;
	switch( p[3] ) /* Meldungsart */
	{
	case 0x01: /* Akkufehler */
		uiErrInfo[ BMA_ERROR ] |= BAL_BMA_AKKUFEHLER;
		break;

	case 0x02: /* Akkufehler weg */
		uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_AKKUFEHLER;
		break;

	case 0x03: /* Netzfehler */
		uiErrInfo[ BMA_ERROR ] |= BAL_BMA_NETZFEHLER;
		break;

	case 0x04: /* Netzfehler weg */
		uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_NETZFEHLER;
		break;

	case 0x0F:
		dgb1 = 5;  
		/* 
		** Routine von ID fehlt 
		*/
		if( id == 0xFFFFFFFF )  /* keine ID vorhanden */
		{
			return( FALSE );
		}
		
		dgb1 = 6; 

		for( i=0; i<BmaUsers.iCount;i++)
		{
			/* Wenn Teilnehmer vorhandenn -> Flag setzen */
			if( BmaUsers.BmaUser[i].id == id )
			{
				BmaUsers.BmaUser[i].RoutineMissing = 1;
			}
		}
		break;

	default:
		return( FALSE );
	}
	return( TRUE );
}


BOOLEAN VdsDecodeSlotFrame(VDSFrame *pFrame)
{
	unsigned char *p;
	int len = pFrame->Data[0];
	
	/*
	** Nutzerdatensatz 0x00 überspringen. Länge des 
	** Datensatzes steht in pFrame->Data[0]
	*/ 
	p = &(pFrame->Data[	len + 2 ]); /* +2 Wegen Size und Types */
	
	dgb1 = 1;

	if( p[0] != 0x02 )    /* Länge */
		return FALSE;
	
	dgb1 = 2;
	if( p[1] != 0xBB )    /* Satztype */
		return FALSE;     /* Nur 0xBB Typen ( Slotinformation ) */

	dgb1 = 3;
	switch( p[2] )
	{
	case 0x20:  /* Fehlanruf eingehend */
		return FALSE;

	case 0x21:  /* Fehlanruf ausgehend */
		return FALSE;

	case 0x22:  /* Meldung Slot mit RN */
	case 0x23:  /* Meldung Slot ohne Rufnummer */
		/* contiue decoding */
		break;

	case 0x24:  /* Meldung richtig quitiert */
		return FALSE;
		break;

	case 0x25:  /* Meldung anzeigen */
		/* contiue decoding */
		break;

	default:
		return FALSE;
	}

	dgb1 = 4;
	switch( p[3] )
	{
		/*
		** Alarmbit setzten
		*/
	case 0xBD:    /* GSM falsche Pinnummer  */
	case 0xBE:    /* Puknummer erforderlich */
	case 0xBF:    /* Simkarte defekt */
		break;

	case 0x38:	  /* Schicht 1 Fehler */
		uiErrInfo[ BMA_ERROR ] |= BAL_BMA_SCHICHT1_FEHER;
		break;

	case 0xB8:		/* Schicht 1 fehler weg */
		uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_SCHICHT1_FEHER;
		break;
    /*
	** AKKU FEHLER  
	*/
	case 0x73:		
		break;

	case 0xF3:
		uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_AKKUFEHLER;
		break;

	/*
	** NETZ FEHLER  
	*/
	case 0x72:		
		uiErrInfo[ BMA_ERROR ] |= BAL_BMA_NETZFEHLER;
		break;

	case 0xF2:
		uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_NETZFEHLER;
		break;

	
	default:
		return( FALSE );    /* Alle  anderen werden nicht decodiert */
	}

	return( TRUE );
}

BOOLEAN VdsCheckBmaUser( TBmaUser *pUser, unsigned char *pFrameStart, unsigned char bFrameLength )
{
/*	BOOLEAN ret = FALSE;*/
	int id;
	unsigned short wMLChanged;
	unsigned short wMLState;
	unsigned short wMLMask;
	unsigned char bSignalText;
	
	dgb1 = 2;
	if( pUser->Disabled )
	{
		return( FALSE );
	}
	
	dgb1 = 3;
	if( pFrameStart[0] != 0x0B )	/* Frame Länge */
	{	
		return( FALSE );
	}
	
	dgb1 = 4;
	if( pFrameStart[1] != 0xB3 )    /* Frame Start */
	{
		return( FALSE );
	}
	
	dgb1 = 5;
	if( pFrameStart[2] != 0x28 )    /* Start BYte */
	{
		return( FALSE );
	}
	
	dgb1 = 6;
	if( pFrameStart[3] != 0x8 )    /* Length  */
	{
		return( FALSE );
	}

	dgb1 = 7;
	/* OK Mybe its a Frame */
	id = GetBcdValue( pFrameStart[5] ) * 1000 + GetBcdValue( pFrameStart[6] ) * 100 + GetBcdValue( pFrameStart[7] );
	if( (pUser->wActivation & ID_MUST_MATCH) && (pUser->id != (unsigned int) id))
	{
		return( FALSE );
	}
	dgb1 = 8;
	/*
	** Status der Meldelinien
	*/
	wMLState = pFrameStart[10] | pFrameStart[11] << 8;

	/*
	** Grund war meldelinienänderung von Linie wMLChanged
	*/
	wMLChanged = pFrameStart[10] >> 4;

	/*
	** 
	*/
	wMLMask = 1 << wMLChanged;

	
	if( (pUser->wActivation & ML_MASK_MUST_MATCH) && ((pUser->wMLMask & wMLMask)==0) )
	{
		return( FALSE );
	}

	dgb1 = 10;
	bSignalText = pFrameStart[8];
	
	if( (pUser->wActivation & SIGNAL_TEXT_MUST_MATCH) && (pUser->SignalText != bSignalText) )
	{
		return( FALSE );
	}

	dgb1 = 99;
	
	return( TRUE );
}

BOOLEAN VdsCheckForActivationFrame(VDSFrame *pFrame, TBmaUsers*pBmaUsers, int *UserToActivate)
{
	int i=0;
	int UserIndex;	

	unsigned char bNDatLength;
	unsigned char bMaskDisabledAlarms;
	unsigned char bAlarmFlags;
	unsigned char bNDatType;
	unsigned char bFrameLength = pFrame->bLen-2;
	RTCtime_typ  rtc;
	
	dgb1 = 99;

	while( i < bFrameLength )
	{
		bNDatLength = pFrame->Data[i];
		if(bNDatLength < 2 )
		{
			return( FALSE );
		}
		bNDatType = pFrame->Data[i+1];
		if( bNDatType == 0xB3 )
		{
			/* 10 Baud Telegram von UG -> Auswerten */
			for(UserIndex = 0; UserIndex < pBmaUsers->iCount; UserIndex++)
			{
				
				if( VdsCheckBmaUser( &(pBmaUsers->BmaUser[UserIndex]), &(pFrame->Data[i]),bNDatLength) == TRUE )
				{
					
					/*
					** Je nach Konfiguration Alarme auslösen
					*/
					bMaskDisabledAlarms = 0x3C;		/* 0011 1100 */

					/*
					LCN         1
					ELA         2
					GRP			4
					SAMMLER		8
					BERG		16
					EDTH		32
					FEUER
					*/

					/*
					** Aktuelle Zeit hohlen
					*/
					RTC_gettime( &rtc );
	
					/*
					** Je nach dem ob gerade Tag ist oder Nacht wird ein 
					** Alarmflag ausgewählt
					*/
					if( rtc.hour >= pBmaUsers->BmaUser[UserIndex].hhDayStart  && 
						rtc.hour <= pBmaUsers->BmaUser[UserIndex].hhDayEnd &&
						rtc.minute >= pBmaUsers->BmaUser[UserIndex].mmDayStart &&
						rtc.minute <= pBmaUsers->BmaUser[UserIndex].mmDayEnd )
					{
						bAlarmFlags = pBmaUsers->BmaUser[UserIndex].AlarmFlagsTag;
					}
					else
					{
						bAlarmFlags = pBmaUsers->BmaUser[UserIndex].AlarmFlagsNacht;
					}

					/* Bit 0 = Standart 1
					   Bit 1 = Zug      2
					   Bit 2 = Grp      4
					   Bit 3 = Berg     8
					   Bit 4 = Edth    10       */

					if( bAlarmFlags & 0x01 )			/* Standart */
					{
						bMaskDisabledAlarms &= ~0x08;		/* Enable Sammel Alarm */
						bMaskDisabledAlarms &= ~0x40;
					}

					/*
					** Wenn Zugsalarm konfiguriert wird er Gruppenalarm 
					** überschrieben
					*/
					if( bAlarmFlags & 0x02 )			/* Zug */
					{
						/*
						** Alarmausgang umkonfigurieren auf Zugsalarm erfolt nur wenn
						** der Alarmausgang nicht gerade aktiv ist.
						*/
						if( IsAlarmOutputInactive( ALARM_OUTPUT_GRP ) )
						{
							CfgAlarmOutputMode( ALARM_OUTPUT_GRP, ALARM_MODE_TWO_OFF_GROUP );
							UpdateOutputMask( ALARM_OUTPUT_GRP );
							bMaskDisabledAlarms &= ~0x04;		/* Enable Zugsalarm Alarm */
						}
					}
					else
					{
						/*
						** Nur wenn kein Zugsalarm gefordert ist kann ein 
						** Gruppenalarm vorhanden sein 
						*/
						if( bAlarmFlags & 0x04 )			/* Grp */
						{
							/*
							** Alarmausgang umkonfigurieren auf Gruppenalarm erfolt nur wenn
							** der Alarmausgang nicht gerade aktiv ist.
							*/
							if( IsAlarmOutputInactive( ALARM_OUTPUT_GRP ) )
							{
								CfgAlarmOutputMode( ALARM_OUTPUT_GRP, ALARM_MODE_ONE_OFF_GROUP );
								UpdateOutputMask( ALARM_OUTPUT_GRP );
								bMaskDisabledAlarms &= ~0x04;		/* Enable Sammel Alarm */
							}
						}
					}

					/*
					**
					*/
					if( bAlarmFlags & 0x08 )			/* Edth						 */
						bMaskDisabledAlarms &= ~0x20;	/* Enable Alarm Bergerndorf  */
					/*
					**
					*/
					if( bAlarmFlags & 0x10 )			/* Berg						 */
						bMaskDisabledAlarms &= ~0x10;	/* Enable Alarm Edtholz      */
					
					/*
					** Aktivieren der neuen Maske für die 
					** Alarmausgänge 
					*/
					SetAlarmDisableMask( BMA_ALARM, bMaskDisabledAlarms );
				
					/*
					** Alarm auslösen
					*/
					RAISE_ALARM( BMA_ALARM )
				}
			}
		}
		
		i+=bNDatLength+2;
	}
	/*
	** 
	*/
	return( TRUE );
}




BOOLEAN VdsDecodeUgMessage(VDSFrame *pFrame)
{
	/*
	** Alle
	*/
	return( FALSE );	
}

/* returns 0-99 for a given bcd value in parameter bcd */
unsigned int GetBcdValue( unsigned char bcd )
{
	return( (unsigned int) (bcd>>4)*10 + (bcd & 0x0F) );
}


int AP_itoa(int value, char* text)
{
	char tmpText[] = {'0', '0', '0', '0','0', '0', '0', '0', '0', '0','0', '0', '0', '0', '0', '0', '0','0', '0', '0',0 };
	BOOLEAN blnNeg = TRUE;
	int iValue = value;
	int i=1;
/*	int iKommaPos;*/
	int j;
	
	if( iValue  == 0 )
	{
		text[i++] = 0;
	}
	/*
	** Wenn der Wert positiv ist Wert *=-1
	*/
	if( iValue  > 0 )
	{
		blnNeg = FALSE;
		iValue = (~iValue)+1;
	}
	/*
	** Für alle Zeichen
	*/
	while( iValue )
	{
	    tmpText[i] = ~(iValue % 10)+49;
		iValue /=10;
		i++;
	}
	/*
	** Vorzeichen bei negativen Zahlen einfügen
	*/
	if( blnNeg == TRUE )
	{
		tmpText[i++] = '-';
	}
	/*
	** Zurückcopieren
	*/
	for(j=i ;i>0; i--)
	{
		text[j-i] = tmpText[i-1];
	}
	return( j -1 );
}

BOOLEAN VdsNDatFrame(FRM_xopen_typ *pDevice, VDSFrame *pFrame)
{
	unsigned char 	bSLength;
	int day,month,year;
	int hour,min,sec;
	int MsgNumber;
	unsigned char bChanel;
	unsigned char bCallingNumberLength;

	int ActiveUser;

	BOOLEAN ret = FALSE;
	
    /*
	** Von der ÜZ kommt immer ein Satztype 0x00 als erster NDat Frame
	** abhängig von den enthaltenen Daten erfolgt eine weitere Decodierung
	*/
	if( pFrame->bLen <= 2 )
	{
		return( FALSE );		/* Dies ist ein Nutzerdatensatz mit keinen Nutzerdaten */
	}

	

	switch( pFrame->Data[1] )
	{
	case FRAME_TYPE_INTERNAL:	/* Nutzerdatensatz  */

		bSLength = pFrame->Data[0];
		
		MsgNumber =  pFrame->Data[4] *255*255 + pFrame->Data[5]*255 + pFrame->Data[6];

		year = GetBcdValue(pFrame->Data[7]) + GetBcdValue(pFrame->Data[8])*100;
		month = GetBcdValue(pFrame->Data[9]);
		day = GetBcdValue(pFrame->Data[10]);
		hour = GetBcdValue(pFrame->Data[11]);
		min = GetBcdValue(pFrame->Data[12]);
		sec = GetBcdValue(pFrame->Data[13]);

		bChanel = pFrame->Data[14];
		
		/*
		** Calling number folgt 
		*/ 
		bCallingNumberLength = pFrame->Data[15];
		
		/*
		** Number
		*/
		switch( pFrame->Data[2] )
		{
		case 0x11: 
			VdsDecodeSystemInternalFrame( pFrame );
			break;

		case 0x12: 
			VdsDecodeSlotFrame( pFrame );
			break;

		case 0x13: 
			dgb1 = 77;
			VdsCheckForActivationFrame(pFrame,&BmaUsers,&ActiveUser);
			break;

		case 0x14: 
			break;

		case 0x15: 
			break;

		case 0x16: 
			break;

		default:
		}
		break;
	
	default:
		/*LOG_EVENT("Ubekannter Satztype");*/

	}
	
	return( ret );
}


#endif
