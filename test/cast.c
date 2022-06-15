//
// Created by qiaojinxia on 2022/6/11.
//
#include "head.h"
int main() {
    assert(131585, (int)8590066177);
    assert(513, (short)8590066177);
    assert(1, (char)8590066177);
    assert(1, (long)1);

    assert(3, ({int ab = 3;(int)*(int *)&ab;}));
//    assert(0, (long)&*(int *)0);
    assert(513, ({ int x=512; *(char *)&x=1; x; }));
    assert(5, ({ int x=5; long y=(long)&x; *(int *)y; }));
//
//    (void)1;
//
    assert(-1, (char)255);
    assert(-1, (signed char)255);
    assert(255, (unsigned char)255);
    assert(-1, (short)65535);
    assert(65535, (unsigned short)65535);
    assert(-1, (int)0xffffffff);
    assert(0xffffffff, (unsigned)0xffffffff);

    assert(1, -1<1);
    assert(0,  -1<(unsigned)1);
    assert(254, (char)127+(char)127);
    assert(65534, (short)32767+(short)32767);
    assert(-1, -1>>1);
    assert(-1, (unsigned long)-1);
    assert(2147483647, ((unsigned)-1)>>1);
    assert(-50, (-100)/2);
    assert(2147483598, ((unsigned)-100)/2);
    assert(9223372036854775758, ((unsigned long)-100)/2);
    assert(99, ((long)-1)+(unsigned)100);
    assert(0, ((long)-1)/(unsigned)100);
    assert(-2, (-100)%7);
    assert(2, ((unsigned)-100)%7);
    assert(6, ((unsigned long)-100)%9);

    assert(65535, (int)(unsigned short)65535);
    assert(65535, ({ unsigned short x = 65535; x; }));
    assert(65535, ({ unsigned short x = 65535; (int)x; }));

    assert(-1, ({ typedef short T; T x = 65535; (int)x; }));
    assert(65535, ({ typedef unsigned short T; T x = 65535; (int)x; }));

    assert(0, (_Bool)0.0);
    assert(1, (_Bool)0.1);
    assert(3, (char)3.0);
    assert(1000, (short)1000.3);
    assert(3, (int)3.99);
//    assert(2000000000000000, (long)2e15);
    assert(3, (float)3.5);
    assert(5, (double)5.5);
    assert(3, (float)3);
//    assert(3, (double)3);
//    ASSERT(3, (float)3L);
//    ASSERT(3, (double)3L);
//
//    printf("OK\n");
    return 0;
}
