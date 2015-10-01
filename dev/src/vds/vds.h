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





#define NDAT_STATUS				0x20
#define NDAT_INTERN				0x00
#define NDAT_BLOCKSTATUS		0x24
#define NDAT_UEGMSG     		0xB3
#define NDAT_SLOTINFORMTION		0xBB
#define NDAT_SYSTEMINTERNAL		0xB


typedef struct
{
    unsigned char bLen        __attribute__((packed));
    unsigned char bType       __attribute__((packed));
    unsigned char Data[255]   __attribute__((packed));
}TVDSUserFrame;

class vds
{
    enumFrameReceiveState enFrameReceiveState;
    unsigned int uiFramesReceived;
    VdsFrame* pFrameReceive;
public:
    vds();
    void ReceiveFrameStateMachine(unsigned char Data);
    void SendAck();
    int ParseUserFrame(unsigned char *pFrameStart);
};

#endif // VDS_H
