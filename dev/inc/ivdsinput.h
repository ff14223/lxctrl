#ifndef IVDSINPUT
#define IVDSINPUT

class IVdsInput
{
public:
    virtual  void ReceiveFrameStateMachine(unsigned char Data)=0;
    virtual  int getFrameReceiveCount()=0;
    virtual  int getFrameErrorCount()=0;
};

#endif // IVDSINPUT

