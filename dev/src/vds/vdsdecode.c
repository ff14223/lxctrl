#if 0
#include "VdsFrame.h"
#include "global.h"

_GLOBAL unsigned int IDwas;						/* Debug Variablen */
unsigned int GetBcdValue( unsigned char bcd );

/* {group:VDS_DECODE}

	Description: Dekodiert einen Frame mit der ID 0x56. ( ID )
	
	Remarks: Es wird nur die enthaltenen ID ( 6 Stellig ) in die pUserData Struktur
	     eingetragen. 
	     
	     ID Ziffern 5, 6 - Data[0]
	     ID Ziffern 3, 4 - Data[1]
	     ID Ziffern 1, 2 - Data[2]
*/
void VdsParse_NDAT_0x56(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
	#if 0 
	char LogText[200];
	int i;

	/*
	   Eintrag ins Logbuch.
	*/
    LogText[0] = 'd';
	LogText[1] = 'x';
	LogText[2] = '5';
	LogText[3] = '6';
	LogText[4] = ':';
	
	for(i=0;i<bLen;i++)
	{
		 HexValue( Data[i], &(LogText[i*2+5]) );
	}
	LogText[i*2+5] = 0;
	LogWriteString( (UDINT) LogText );
	#endif
	/*
	   Die nibbles sind geswapt. Für ID:13 wird 31 in Data[2]
	   gesendet.
	   
	   ACHTUNG: Wahrscheinlich ein Fehler in der ÜZ. Ist bei einem Update besonders zu 
	   beachten.
	   
	   Die ID wird nur für Anzeige verwendet. Ein Störungsalarm wird bei jedem Eintrefen einer
	   Routine von ID fehlt Nachricht ausgelöst. (VdsParse_NDAT_SYSTEMINTERNAL)
	*/
    Data[2] = ((Data[2]<<4) & 0xF0) | ((Data[2]>>4) & 0x0F );
    Data[1] = ((Data[1]<<4) & 0xF0) | ((Data[1]>>4) & 0x0F );
    Data[0] = ((Data[0]<<4) & 0xF0) | ((Data[0]>>4) & 0x0F );
    
    /*
        Read the ID
    */
	pUserData->uiID = GetBcdValue( Data[0] )*10000 + GetBcdValue( Data[1] )*100 + GetBcdValue( Data[2] );

}

/* {group:VDSDECODE}

   Description:
   
   Remarks:
   
*/
void VdsParse_NDAT_SYSTEMINTERNAL(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
#if 0
	TBmaUser *pUser;


	char txtRoutineFehlt[] = {"ARoutine fehlt von Teilnehmer:                                         "};
	int j,i;
	char LogText[200];
	int i;
    LogText[0] = 'd';
	LogText[1] = 'Y';
	LogText[2] = 'S';
	LogText[3] = 'I';
	LogText[4] = ':';
	
	for(i=0;i<bLen;i++)
	{
		 HexValue( Data[i], &(LogText[i*2+5]) );
	}
	LogText[i*2+5] = 0;
	LogWriteString( (UDINT) LogText );
#endif
	
	switch( Data[1] )
	{
		case 0x00: 
			LogWriteString((UDINT)"LOG::202"); 	
			break;
			
		case 0x0F: 
			/*
			** Auf alle Fälle merken
			*/
			ucRoutineFehlt = 1;
#if 0
			/* 
			** Mit allen Usern vergleichen
			*/
			for(i = 0; i< BmaUsers.iCount; i++)
			{
				pUser = &(BmaUsers.BmaUser[i]);

				/*
				** User ist disabled
				*/
				if( pUser->Disabled )
					continue;

				/* 
				** Check ID
				*/
				if( pUser->id == pUserData->uiID)
				{
					j=0;
					for(j=0;pUser->Name[j];j++)
					{
						txtRoutineFehlt[30+j] = pUser->Name[j];
					}
					LogWriteString((UDINT)txtRoutineFehlt);					     
					return;
				}
			}
#endif
			/* Meldung wird vom Windows Rechner erzeugt */
			/* LogWriteString((UDINT)"ARoutine von ID:????? fehlt "); */
			break;
			
		case 0x01:
			LogWriteString((UDINT)"LOG::231");
			/* uiErrInfo[ BMA_ERROR ] |= BAL_BMA_AKKUFEHLER;			 */
			break;

		case 0x02:
			LogWriteString((UDINT)"LOG::232");
			/* uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_AKKUFEHLER;			 */
			break;
			
		case 0x03:
			LogWriteString((UDINT)"LOG::233");
			uiErrInfo[ BMA_ERROR ] |= BAL_BMA_NETZFEHLER;			
			break;

		case 0x04:
			LogWriteString((UDINT)"LOG::234");
			uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_NETZFEHLER;			
			break;

	}
}

/* {group:VDSDECODE}

    Description: Auswerten eines Frames mit dem Satztype 0x00.
    
    Remarks:
    
    Aufbau
    Art - Data[0]
    Regisitrierung - Data[1]
    Laufende Nummer im Meldungsp. - Data[2,3,4] ( Binär )
    Eintreffen der Meldung Datum & Zeit - Data[5,6,7,8,9,10,11]
    Kanal und Adresse -  Data[12]
    
*/    
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
	
	pUserData->uiNumber = 0;/* GetBcdValue( Data[2] )*10000 + GetBcdValue( Data[3] )*100 + GetBcdValue( Data[4] );*/
	pUserData->usKanal     =  Data[12] >> 4;
	pUserData->usAddresse  =  Data[12] & 0x0F;	
	pUserData->usTransport =  Data[13]; 
}


void VdsParse_NDAT_STATUS(unsigned char bSize, unsigned char*Data, TVDSUserData *pUserData)
{
	/* LogWriteString((UDINT)"dType:NDAT_STATUS");	*/
}

/* {group:VDS_GetBcd
}

   Description: Decodiert eine Frame für Slotinformationen.
   
   Remarks:
*/
void VdsParse_NDAT_SLOTINFORMTION(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData)
{
	switch( Data[1] )
	{
		case 0x38:
			uiErrInfo[ BMA_ERROR ] |= BAL_BMA_SCHICHT1_FEHER;
			bISDNBusStoerung = 1;
			/* LogWriteString((UDINT)"ASchicht 1 Fehler (ISDN S0)"); */
			break;
			
		case 0xB8:
			uiErrInfo[ BMA_ERROR ] &= ~BAL_BMA_SCHICHT1_FEHER;
			bISDNBusStoerung = 0;
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


/* {group:VDS_DECODE}
  
   Description: Dekodiert einen Frame von einem Übertragungsgerät.
  
   Remarks: Satztyp 0xB3
  
   Start                        -  0x28  Data[0]
   Anzahl folgender Bytes       -  0x08
   Gerätetyp                    -  0xXX
   Identnummer                  -  0xXX  Data[3]
   Identnummer                  -  0xXX
   Identnummer                  -  0xXX
   Signaltyp                    -  0xXX  Data[6]
   Grund/Störung                -  0xXX
   Zustand der Meldelinien 1-8  -  0xXX  Data[8]
   Zustand der Meldelinien 9-16 -  0xXX
   Checksumme                   -  0xXX
  
  
   Start                        -  0x28
   Anzahl folgender Bytes       -  0x01
   Gerätetyp                    -  0xXX


 *Geräte- Typ*

   @table
   Hex    ASCII   Bemerkung
   ---    -----   -------------
   0x34   4       S2
   0x36   6       RD
   0x37   7       T 508 D... / RS1 / 2000 RS
   0x38   8       T 7008 D / T 508 D... mit Al- Option / RS 2
   0x39   9       T 7008 D-AN / T 7008 D
   0x41   A       T 7008 D-AN / T 508 DF/1 / T 508 DF / 4
   0x46   F       T 7008 D-AN / T 608 DFA mit Fernschalten
   0x47   G       T 7008 D-AN / T 608 DF mit Fernschalten
   0x53   S       S 7016 FS


 *Signal- Typ*

   @table
   Hex    ASCII   Bemerkung
   ---    -----   -------------
   0x31   ”1”     Sondertext 1
   ....   ...     ....
   0x38   ”8”     Sondertext 8
   0x41   ”A”     ALARM
   0x46   ”F”     FEHLER
   0x4B   ”K”     KLAR
   0x4C   ”L”     TECHN. ALARM
   0x4E   ”N”     NOTRUF
   0x4F   ”O”     RUF (nur bei S2-Gerät, Meldung nach Anruf)
   0x51   ”Q”     FS-EIN
   0x52   ”R”     ROUTINE
   0x53   ”S”     FS-AUS
   0x54   ”T”     SCHARF
   0x55   ”U”     UNSCHARF
   0x57   ”W”     WATCHDOG ABGESTELLT (bei Inbetriebnahme
   0x3C   ”<”     RÜCKRUF (Grund G: ”<”)

   
 *Byte- Nr. 8: Störungen  niederwertiges Halbbyte*

   @table
   Bit   Bemerkung
   ----  ---------
   0 =1  Fehler AKKU (z. B. mit Signal- Typ ”F”)
   1 =1  Fehler NETZ (z. B. mit Signal- Typ ”F”)
   2 =1  Fernsprechleitung gestört (z. B. mit Signal- Typ ”F”; T-STOER)
   3 =1  Pegelfehler (bei T 508 DA)


 *Grund höherwertiges Halbbyte*

   @table
   Grund   Bemerkung
   -----   ---------
   0       Grund der Auslösung ist keine Meldelinie 24 h Routine / 
           Fernschaltrückmeldung / Fehler – AKKU / Fehler NETZ / KLAR
           (bei DF- Geräten und wenn NETZ- AKKU- Fehler beseitigt ist)
   1..15   Grund der Auslösung ist die Meldelinie

  Byte- Nr. 9  - Zustand der Meldelinien ML 1 ... ML 8
  Byte- Nr. 10 - Zustand der Meldelinien ML 9 ... ML 16
  Byte- Nr. 11 - Checksumme Summe ohne das Byte- Nr. 1 Summe ohne Übertrag

*/
void VdsParse_NDAT_UEGMSG(unsigned char bSize, unsigned char*Data, TVDSUserData *pUserData)
{
	unsigned int id;
	int UserIndex;
	TBmaUser *pUser;
	unsigned short wMLChanged;
	unsigned short wMLState;
	unsigned short wMLMask;
	unsigned char bSignalText;
	
#if 0	
	char LogText[200];
	int i;

    LogText[0] = 'd';
	LogText[1] = 'S';
	LogText[2] = 'L';
	LogText[3] = 'T';
	LogText[4] = ':';
	
	for(i=0;i<bSize;i++)
	{
		 HexValue( Data[i], &(LogText[i*2+5]) );
	}
	LogText[i*2+5] = 0;
	LogWriteString( (UDINT) LogText );
#endif

	if( Data[0] != 0x28 )
	{
		return;
	}

	if( Data[1] == 1 )
	{
		return;
	}

	/*
	** ID
	*/
	id = GetBcdValue( Data[5] );
	id += GetBcdValue( Data[4] ) * 100;
	id += GetBcdValue( Data[3] ) * 10000;

	IDwas = id;
	
	/*
	** Status der Meldelinien
	*/
	wMLState = Data[8] | Data[9] << 8;

	/*
	** Grund war meldelinienänderung von Linie wMLChanged
	*/
	wMLChanged = Data[7] >> 4;

	if( wMLChanged == 0 )    /* in diesem Fall ware es keine Alarmmeldung */
		return;
		
	/*
	** 
	*/
	wMLMask = 1 << (wMLChanged-1);
	
	bSignalText = Data[6];
	
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
		
		/* LogWriteString((UDINT)"BMA Alarm auslösen.."); */
		VdsActivateBMZTeilnehmer(pUser);
	}
}

/* {group:VDS_DECODE}
**
** Description: Liefert den Wert einer BCD Zahl.
**
** Remarks: Der Wert der in BCD angegebenen zweistelligen Zahl wird in einen
**    binären Wert umgewandelt. zb 0x99 auf 99
**
** Returns: Wert einer BCD Zahl.
*/
unsigned int GetBcdValue( unsigned char bcd )
{
	return( (unsigned int) ((bcd>>4)*10 + (bcd & 0x0F)) );
}
#endif
