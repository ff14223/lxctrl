#ifndef VDSFRAME_H
#define VDSFRAME_H

#define OFFSET_LENGTH   1
#define OFFSET_STEUER   3
#define OFFSET_ADDR     4
#define OFFSET_DATA     5
#define OFFSET_CHECKSUM 6

typedef int (*cbUserFrame)(int len, unsigned char *pStart, unsigned int cbCooky);

class VdsFrame
{
    unsigned char m_Data[258];
    unsigned char *pData;
public:
    VdsFrame();
    void AddData(unsigned char data);
    void Reset();
    int Length();
    unsigned char Checksumm();
    void SetA(unsigned char c);
    void SetC(unsigned char c);
    unsigned char getC(){ return m_Data[OFFSET_STEUER]; }
    unsigned char getA(){ return m_Data[OFFSET_ADDR];}
    void ForEachUserFrame( cbUserFrame cb, unsigned int cbCooky );
    int GetNDatFrame(int *pOffset, unsigned char *pData);
};

#endif // VDSFRAME_H
