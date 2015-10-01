#ifndef TEST_H
#define TEST_H

class Test
{
public:
    Test();
    void testVdsFrame();
    void AssertEqual(int a, int b, char *what);
    void SimpleFrame();
    void TestFrame();
};

#endif // TEST_H
