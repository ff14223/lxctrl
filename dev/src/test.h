#ifndef TEST_H
#define TEST_H

#include <src/vds/vds.h>

class Test
{
public:
    Test();
    void testVdsFrame(vds *pVds);
    void AssertEqual(int a, int b, char *what);
    void SimpleFrame();
    void TestFrame(vds *pVds);
    void LoadSimulationFile(vds *pVds,const char *FileName);
};

#endif // TEST_H
