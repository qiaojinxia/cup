//
// Created by qiaojinxia on 2022/6/11.
//
#include "head.h"
int main() {
    { const x; }
    { int const x; }
    { const int x; }
    { const int const const x; }
    assert(5, ({ const x = 5; x; }));
    assert(8, ({ const x = 8; int *const y=&x; *y; }));
    assert(6, ({ const x = 6; *(const * const)&x; }));

    return 0;
}
