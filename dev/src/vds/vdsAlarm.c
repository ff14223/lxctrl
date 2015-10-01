#if 0
#include "VdsFrame.h"
#include "global.h"
#include "BMAUsers.h"
#include "io.h"


/* {group:VDS}
 
   Description: Aktiviert einen Alarm für einen BNZ Teilnehmer.

   Remarks: Die Alarme die beim Teilnehmer angeschlossen sind werden
         ausgelöst.

   Parameters:
        pBmaUser - Zeiger auf BMZ Teilnehmerstruktur.

*/
void VdsActivateBMZTeilnehmer( TBmaUser *pBmaUser )
{
	unsigned char bMaskDisabledAlarms;
	unsigned char bAlarmFlags;
	RTCtime_typ  rtc;
	int Min1,Min2;
    /*char Text[] = {"ABMA Alarm Teilnehmer:                                      "};*/

	/*
	** Je nach Konfiguration Alarme auslösen.
	** Die Bedeutung der Bits ist abhängig von 
	** der Reihenfolge der Alarm im BMA Alarm (Alarmtask)
	**
	** Bit 
	** 0 - LCN
	** 1 - ELA
	** 2 - GRP
	** 3 - SAMMLER
	** 4 - BERG
	** 5 - EDTH
	** 6 - SAMMLER (nach 3 min)
	** 7 - Sirene
	**
	** Alarmausgänge die hier auf 1 ( disabled sind) werden je nach Konfiguration und 
	** aktueller Zeit wieder enabled (auf 0 gesetzt).
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

	
	bAlarmFlags = pBmaUser->AlarmFlagsNacht;
	/*
	** Je nach dem ob gerade Tag ist oder Nacht wird ein 
	** Alarmflag ausgewählt
	*/
	Min1 = rtc.hour *60 + rtc.minute;
	Min2 = pBmaUser->hhDayStart*60 + pBmaUser->mmDayStart;

	if( Min1 >= Min2 )
	{
		Min2 = pBmaUser->hhDayEnd*60 + pBmaUser->mmDayEnd;
		if( Min1 <= Min2 )
		{
			bAlarmFlags = pBmaUser->AlarmFlagsTag;
		}
	}
	
	/*
	** always include BERG und EDTH in case of an alarm
	*/
	
	/* Bedeutung der Bits in den AlarmFlags
	   Bit 0 = Sammler
	   Bit 1 = Zug     
	   Bit 2 = Grp    
	   Bit 3 = Edth   	   
	   Bit 4 = Berg   
	*/
	if( bAlarmFlags & 0x01 )			/* Standart */
	{
		bMaskDisabledAlarms &= ~0x08;		/* Enable Sammel Alarm */
	}

	/*
	** Wenn Zugsalarm konfiguriert wird der Gruppenalarm 
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
	** Alarm Am Thalbach
	*/
	if( bAlarmFlags & 0x08 || bAlarmFlags & ALF_BERGENDORF )	/* Edth	*/
		bMaskDisabledAlarms &= ~0x10;	/* Enable Alarm Am Thalbach	*/
		
	/*
	** Bergerndorf
	*/
	/*if( bAlarmFlags & 0x10 )			
		bMaskDisabledAlarms &= ~0x10;	*/
	
	/*
	** Aktivieren der neuen Maske für die 
	** Alarmausgänge 
	*/
	SetAlarmDisableMask( BMA_ALARM, bMaskDisabledAlarms );

	/*
	** Alarmdurchsage 
	*/
	ConfigAlarmSound( BMA_ALARM, (UDINT) (pBmaUser->id), (UDINT)&outEnableNFInBMA);
	
	/*
	** Alarm auslösen
	*/
	RAISE_ALARM( BMA_ALARM , tInterventionTime, (UDINT) "LOG::414" )
}
#endif
