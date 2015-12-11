#ifndef VDSFRAME_H
#define VDSFRAME_H



typedef int (*cbUserFrame)(int len, unsigned char *pStart, unsigned int cbCooky);

class VdsFrame
{
    unsigned char m_Data[258];
    unsigned char *pData;
    unsigned int uiCheckSum;
public:
    VdsFrame();
    void AddData(unsigned char data);
    void Reset();
    int Length();
    unsigned char Checksumm();
    void SetA(unsigned char c);
    void SetC(unsigned char c);
    unsigned char getC();
    unsigned char getA();
    void ForEachUserFrame( cbUserFrame cb, unsigned int cbCooky );
    int GetNDatFrame(int *pOffset, unsigned char *pData);
    void Dump();
};

#endif // VDSFRAME_H
