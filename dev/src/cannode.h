#ifndef CANNODE_H
#define CANNODE_H


class CanNode
{
    int m_NodeNr;

public:
    CanNode(int NodeNr);
    int getNodeNumber(){ return m_NodeNr; }
};

#endif // CANNODE_H
