#ifndef VDS_H
#define VDS_H

#include "src/vdsframe.h"
#include "inc/ivdsinput.h"

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

class vds : public IVdsInput
{
    enumFrameReceiveState enFrameReceiveState;
    VdsFrame* pFrameReceive;
    IDatabase *pIDb;
    ISystemSignals *pSignals;
    ISystem *pSystem;
    TVDSUserData m_data;
    int fdSerialport;
    int iFramesreceived;
    int iFrameerrors;
    int cbFrame(int Len, unsigned char *pData, unsigned int cooky);
    void VdsParse_NDAT_INTERN(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_SLOTINFORMTION(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_SYSTEMINTERNAL(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_0x56(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_0x52(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
    void VdsParse_NDAT_UEGMSG(unsigned char bLen, unsigned char*Data, TVDSUserData *pUserData);
public:
    vds(IDatabase*pIDb, ISystemSignals *pSignals, ISystem *pSystem, int fd);
    void ReceiveFrameStateMachine(unsigned char Data);
    void SendAck();

    void SendNorm();
    int ParseUserFrame(unsigned char *pFrameStart);

    int getFrameReceiveCount(){return iFramesreceived;}
    int getFrameErrorCount(){return iFrameerrors;}
};


unsigned int GetBcdValue( unsigned char bcd );

#endif // VDS_H
