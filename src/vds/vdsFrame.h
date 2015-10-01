
#ifndef _VDS_FRAME_H_
#define _VDS_FRAME_H_

#include <bur/plctypes.h>   
#include "dvframe.h"


#define FCMD_SEND_NORM			0
#define FCMD_SEND_NDAT			3


#define NDAT_STATUS				0x20
#define NDAT_INTERN				0x00
#define NDAT_BLOCKSTATUS		0x24
#define NDAT_UEGMSG     		0xB3
#define NDAT_SLOTINFORMTION		0xBB
#define NDAT_SYSTEMINTERNAL		0xBF


/*
** Description: Ein ganzer Frame wie er von der ÜZ gesendet wird
**
** Remarks: 
*/
typedef struct
{
	unsigned char bLen;				/* Länge des Frames einsch. C und A Feld und Nutzerdaten */	
	unsigned char C;				/* Steuerfeld        */
	unsigned char A;				/* Address Feld      */
	unsigned char Data[255];		/* Nutzerdaten       */
}TVDSFrame;


/*
** Description: Gesammelte daten von der Frame auswertung. 
**
** Remarks: Die Daten werden vor jedem Frame ( nicht User Frame NULL )
**
*/
typedef struct 
{
	unsigned short usKanal;		/* Kanal der Einsteckkarte   				*/
	unsigned short usAddresse;	/* Addresse der Einsteckkart 				*/
	unsigned int uiNumber;		/* Laufende Nummer der Meldung bei ÜZ 		*/
	unsigned short usReg;		/* wodurch wurde diese Meldung registriert 	*/
	unsigned int Art;			/*  */
	unsigned short usTransport; /* Transportdienst 							*/
	unsigned int uiID;			/*  */
}TVDSUserData;


typedef struct 
{
	unsigned char bLen        __attribute__((packed));
	unsigned char bType       __attribute__((packed));
	unsigned char Data[255]   __attribute__((packed));
}TVDSUserFrame;


/*
** Description: Vds Empfangs Daten 
**
*/
typedef struct 
{
	TVDSFrame    CurrFrame;			/* Der Frame der empfangen wurde  */
	unsigned int  uiState;			/* Der Status der Statemachine    */
	unsigned int  uiDataOffset;		/* Offset beim empfang von Daten  */
	unsigned int  uiCheckSum;		/* Checksummenbildung			  */
	unsigned int  uiRFrameCount;    /* Anzahl der empfangenen Frames  */
	FRM_xopen_typ  SerDevice;		/* Informationen über das serielle 
									   Interface das zu verwenden ist */
	FRM_read_typ   xRead;
	
	FRM_gbuf_typ   xSBuf;			/* Sendebuffer					  */
	FRM_write_typ  FrmWrite;        /* Schreibauftrag				  */
	FRM_robuf_typ  FrmRobuf;		/* Noch ein Buffer				  */
	TVDSUserData   UserData;
	

	/*
	**
	*/
	unsigned int tNoAckSent;		/* Zeit in 10ms in der kein SendAck gesendet wurde */
}TVDS2465;


/*
** VdsFrame Handling
*/
#define VDS_FRAME_WAIT_FOR_START		0
#define VDS_FRAME_GET_LEN				1
#define VDS_FRAME_VERIFY_LEN			2
#define VDS_FRAME_GET_HEADER_END		4
#define VDS_FRAME_GET_CFIELD			5
#define VDS_FRAME_GET_AFIELD			6
#define VDS_FRAME_GET_DATA				7
#define VDS_FRAME_GET_FRAME_END         8
#define VDS_FRAME_GET_CHECKSUM			9

#define RESET_STATE_MACHINE(VDS) VDS->uiState = VDS_FRAME_WAIT_FOR_START	

void VdsSendAck( TVDS2465 *pVds );
void VdsStateMachine(TVDS2465 *pVds, unsigned char Data);

void VdsParse_NDAT_STATUS(unsigned char bSize, unsigned char*Data, TVDSUserData *pUserData);
void VdsParse_NDAT_INTERN(unsigned char bSize, unsigned char*Data, TVDSUserData *pUserData);
void VdsParse_NDAT_SLOTINFORMTION(unsigned char bSize, unsigned char*Data, TVDSUserData *pUserData);
void VdsParse_NDAT_SYSTEMINTERNAL(unsigned char bSize, unsigned char*Data, TVDSUserData *pUserData);
void VdsParse_NDAT_UEGMSG( unsigned char bSize, unsigned char*Data, TVDSUserData *pUserData);
void VdsParse_NDAT_0x56(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);

#endif