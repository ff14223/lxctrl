#ifndef VDS_H
#define VDS_H

#include "src/vdsframe.h"

typedef enum
{
    VDS_FRAME_WAIT_FOR_START		=0,
    VDS_FRAME_GET_LEN				=1,
    VDS_FRAME_VERIFY_LEN			=2,
    VDS_FRAME_GET_HEADER_END		=4,
    VDS_FRAME_GET_CFIELD			=5,
    VDS_FRAME_GET_AFIELD			=6,
    VDS_FRAME_GET_DATA				=7,
    VDS_FRAME_GET_FRAME_END         =8,
    VDS_FRAME_GET_CHECKSUM			=9
}enumFrameReceiveState;


/*typedef struct
{
    unsigned char bLen        __attribute__((packed));
    unsigned char bType       __attribute__((packed));
    unsigned char Data[255]   __attribute__((packed));
}TVDSUserFrame;*/

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

#include "inc/interfaces.h"
#include "inc/ISystemData.h"

class vds
{
    enumFrameReceiveState enFrameReceiveState;
    unsigned int uiFramesReceived;
    VdsFrame* pFrameReceive;
    IDatabase *pIDb;
    ISystemSignals *pSignals;
    ISystem *pSystem;
    TVDSUserData m_data;
    int cbFrame(int Len, unsigned char *pData, unsigned int cooky);
    void VdsParse_NDAT_INTERN(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_SLOTINFORMTION(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_SYSTEMINTERNAL(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_0x56(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_0x52(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_UEGMSG(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
public:
    vds(IDatabase*pIDb, ISystemSignals *pSignals, ISystem *pSystem);
    void ReceiveFrameStateMachine(unsigned char Data);
    void SendAck();

    int ParseUserFrame(unsigned char *pFrameStart);
};


unsigned int GetBcdValue( unsigned char bcd );

#endif // VDS_H
