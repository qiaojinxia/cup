int testCode(){
//    assert(-35,({ 10 - 3  * 4  + 5 * 4 / 3  + 3 - 4  * 2 * 6 +2 * 7 - 7 / 4 *3 - 2 - 1 *2 -2+1+2/3;}));
//    assert(1,({ 10 - 3  * 4  + 5 - 4 / 3  + 3 - 4 ;}));
//    assert(2,({ 10 + 3  - 4  + 5 - 4 / 3  + 3 % 2 - 4 * 3 ;}));
//    assert(11,({2 + 9;}));
//    assert(2,({4 / 2;}));
//    assert(0,({4 < 2;}));
//    assert(1,({4 >= 2;}));
//    assert(1,({7 % 2;}));
//    assert(7,({i = 7;i = i + 1;}));
//    assert(7,({int i = 7;if(i == 7){i;}else{0;}}));
//      assert(13,({int a = 3 ;for(int i = 0;i<10;i=i+1){a = a + 1;}a;}));
//    assert(-8,({ -+10 + 2;}));
//    assert(12,({ --10 + 2;})); //support annotation
//    assert(3,({ int x = 3;*&x;}));
//    assert(9,({ int x,y,z = 3;x + y + z;}));
//    assert(2,({ int m = 3;int *d = &m;m = 2;*d;}));
//    assert(3,({ int x = 3;int y = x;x = 2;y;}));
//    assert(5,({ int x = 3;int y = x;int *z = &x;*(z+1) = 5;y;}));
//    assert(3,({ int x = 3;int y = x;int *z = &x;*(z-1) = 5;y;}));
//    assert(1,({ int x = 3;int y = 4;int *z = &y;int *k = &x;z-k;}));
//    assert(-1,({ int x = 3;int y = 4;int *z = &x;int *k = &y;z-k;}));
//    assert(4,({ int x = 2; int *y = &x ; sizeof(x);}));
//    assert(4,({ int x = 2; int *y = &x ; sizeof y;}));
//    assert(4,({ sizeof(7);}));
//    assert(5,({ int a[3]; int  *x = a ;*x = 7; *(x+1) = 5;*(x+2) = 3; *(a+1); }));
//    assert(7,({ int a[3];a[0] = 1;1[a] = 7;a[2] = 1; a[1];}));
      assert(7,({ char a[3];a[0] = 1;a[1] = 7;a[2] = 1; a[1];}));
//    assert(1,({char x; sizeof(x);}));
//    assert(2,({short y; sizeof(y);}));
//    assert(8,({long z; sizeof(z);}));
    assert(17,({ int x = 1;char y = 2;short z = 5; long m = 9;testFuncAdd(x,y,z,m);}));

    return a;
}

int testFuncAdd(int a,char b,short c,long d) {
    return  a + b + c + d ;
}