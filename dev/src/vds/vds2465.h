#ifndef _VDS2465_H_
#define _VDS2465_H_

#include "global.h"

#define CMD_FLAGS_DIRECTION_OUT 		0x80			/*	*/


#define CMD_MASK					0x0F
#define CMD_SEND_NORM				0x00
#define CMD_CONFIRM_ACK         	0x00
#define CMD_SEND_NDAT           	0x03
#define CMD_CONFIRM_NACK			0x01

#define FRAME_TYPE_INTERNAL			0x00		/* Meldung von UZ7500 */
 
#define FRAME_TYPE_SLOTINFO			0xBB
#define FRAME_TYPE_SYS_INTERNAL     0xBF





#define VDS_STATE_RUNNING			0
#define VDS_STATE_POWER_UP			1
#define VDS_STATE_DISCONNECTED		2
/*
typedef struct 
{
	UINT uiNDat;
	UINT uiSendNorm;
}TFramesReceived;

typedef struct
{
	TFramesReceived FramesReceived;	
	UINT tSendNorm;		
	UINT tNDat;			
	UINT Status;
}TVDS2465;

typedef struct VDSFrame
{
	unsigned char bLen;
	unsigned char Cmd;
	unsigned char Adr;
	unsigned char Data[255];
}VDSFrame;
*/

typedef enum 
{
	RESTART,
	AKKU_FEHLER,
	AKKU_OK,
	NETZFEHLER,
	NETZ_OK,
	DRUCKER_STOERUNG,
	DRUCKER_OK,
	DRUCKER_OFFLINE,
	DRUCKER_ONLINE,
	DRUCKER_OUT_OFF_PAPER,
	DRUCKER_PAPER_OK,
	COM1_OFFLINE,
	COM1_ONLINE,
	COM2_OFFLINE,
	COM2_ONLINE,
	ROUTINE_FROM_ID_MISSING = 0x0F,
	COM2_REG_OFF
}enumSysInternalTypes;


#define INTERNAL			0x11
#define SLOT				0x12
#define S2_S3				0x13
#define S2_S3_RESPONSE		0x14
#define FERNWIRK_RESP		0x15
#define FERNWIRK_REQ		0x16



/*
** Arten der Registrierung
*/
#define REG_KEY				0x01		/* Registriert an der Tastatur */
#define REG_COM1			0x02		/* Registriert über COM1 ( S4-Protokoll ) */
#define REG_COM2			0x04		/* Registriert über COM2 Steuerleitung */
#define REG_ALL_AUTO		0x08		/* Automatisch all von UZ */
#define REG_DISPLAY			0x10		/* wurde am Display ausgegeben */
#define REG_HIST_LPT		0x20		/* wurde auf LPT ausgegeben */
#define REG_HIST_COM1		0x40		/* wurde auf COM1 ausgegeben */
#define REG_HIST_COM2		0x80		/* wurde auf COM2 ausgegeben */

#define TRANS_INTERN		0x00
#define TRANS_ANALOG_FEST	0x10
#define TRANS_ANALOG		0x20
#define TRANS_X25_D			0x30
#define TRANS_X31_ASCII		0x34
#define TRANS_31_EFF_EFF    0x38
#define TRANS_ISDN_B		0x40
#define TRANS_ISDN_D		0x50
#define TRANS_BETRIEBS_FUNK	0x60
#define TRANS_DATEN_FUNK    0x70
#define TRANS_MOBILFUNK     0x80

BOOLEAN InitVdsFrameHandling(void);
BOOLEAN HandleVdsFrame(FRM_xopen_typ *pDevice, VDSFrame *pFrame);



#define SER_LOG_SIZE			1024





#endif