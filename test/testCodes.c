int testCode(){
//    assert(-35,({ 10 - 3  * 4  + 5 + 4 / 3  + 3 - 4  * 2 * 6 +2 * 7 - 7 / 4 *3 - 2 - 1 *2 -2+1+2/3;}));
//    assert(14,({  2  +  3 * 5 - 3 ;}));
//    assert(1,({ 10 - 3  * 4  + 5 - 4 / 3  + 3 - 4 ;}));
//    assert(2,({ 10 + 3  - 4  + 5 - 4 / 3  + 3 % 2 - 4 * 3 ;}));
//    assert(16,({ 10 + 3  / 4  + 5 * 4 / 3 ;}));
//    assert(11,({2 + 9;}));
//    assert(2,({4 / 2;}));
//    assert(0,({4 < 2;}));
//    assert(1,({4 >= 2;}));
//    assert(1,({7 % 2;}));
//    assert(3,({int i = 2;if(i % 2 == 0){i = i + 1;} i;}));
//    assert(8,({int i = 7;i = i + 1;}));
//    assert(7,({int i = 7;if(i == 7){i;}else{0;}i;}));
//    assert(13,({int a = 3 ;for(int i = 0;i<10;i=i+1){a = a + 1;}a;}));
//    assert(-8,({ -+10 + 2;}));
//    assert(12,({ --10 + 2;}));
//    assert(3,({ int x = 3;*&x;}));
//    assert(9,({ int x,y,z = 3;x + y + z;}));
//    assert(2,({ int m = 3;int *d = &m;m = 2;*d;}));
//    assert(3,({ int x = 3;int y = x;x = 2;y;}));
//    assert(5,({ int x = 3;int y = x;int *z = &x;*(z+1) = 5;y;}));
//    assert(3,({ int x = 3;int y = x;int *z = &x;*(z-1) = 5;y;}));
//    assert(1,({ int x = 3;int y = 4;int *z = &y;int *k = &x;z-k;}));
//    assert(-1,({ int x = 3;int y = 4;int *z = &x;int *k = &y;z-k;}));
//    assert(4,({ int x = 2; int *y = &x ; sizeof(x);}));
//    assert(8,({ int x = 2; int *y = &x ; sizeof y;}));
//    assert(4,({ sizeof(7);}));
//    assert(5,({ int a[3]; int  *x = a ;*x = 7; *(x+1) = 5;*(x+2) = 3; *(a+1); }));
//    assert(7,({ int a[3];a[0] = 1;1[a] = 7;a[2] = 1; a[1];}));
//    assert(7,({ char a[3];a[0] = 1;a[1] = 7;a[2] = 1; a[1];}));
//    assert(1,({char x; sizeof(x);}));
//    assert(2,({short y; sizeof(y);}));
//    assert(8,({long z; sizeof(z);}));
//    assert(17,({ int x = 1;char y = 2;short z = 5; long m = 9;testFuncAdd(x,y,z,m);}));
//    assert(2,({int m = 1;{m = 2;} m;}));
//    assert(16,({struct {char name;int group;int num;int age;} stu;sizeof (stu);}));
//    assert(7,({struct {char *name;char group;int num;int age;} stu,stu1; stu.group = 7;stu.num = 1;stu.group;}));
//    assert(5,({struct Stu {char *name;char group;int num;int age;} stu; struct Stu *s = &stu;s->name = 5;s->group = 5;s->name=&s->group;s->age=8;*s->name;}));
//    assert(5,({int a = 3 ;for(int i = 0;i<10;i=i+1){ if(a >= 5) {break;}else{a = a + 1;}}a;}));
//    assert(0,({int a = 3 ;while(a <= 100){ a = 0; break;}a;}));
//    assert(101,({int i = 100;do {i = i + 1;}while(i < 100);i;}));
//    assert(303,({int a = 3; for(int i = 0;i<100;i=i+1){ a = a + 1;a = a + 2;} a;}));
//    assert(103,({int a = 3; for(int i = 0;i<100;i=i+1){ a = a + 1;continue;a = a + 2;} a;}));
//    assert(34,({int i = 0;int count = 0 ; while(i<100){count = count + 1;i = i + 1;i = i + 2;} count;}));
//    assert(100,({int i = 0;int count = 0 ; while(i<100){count = count + 1;i = i + 1;continue;i = i + 2;} count;}));
//    assert(100,({int i = 0;int count  = 0;do {i = i + 1;count = count + 1;continue;i = i + 2;} while (i < 100);count;}));
//    assert(34,({int i = 0;int count  = 0;do {i = i + 1;count = count + 1;i = i + 2;} while (i < 100);count;}));
//    assert_f(3.234567,({float num = 1.234567;float num1 = 2.0;num + num1 + 1.0 + 3.0;}));
//    assert_f(1.0,({float num = 2.0; float num1 = 1.0;num - num1;}));
//    assert_f(2.469134,({float num = 1.234567;float num1 = 2.0;num * num1;}));
//    assert_f( 1.117284,({float num = 1.234567;float num1 = 2.0;num / num1 + 0.5;}));
//    assert_f(0.617284,({float num = 2.234567;float num1 = 2.0;num + num1;}));
//    print_lfloat(({double num1 = 2.0;float num2 = 1.4; num1 + num2;}));
//    print_lfloat(({double num1 = 2.0;double num2 = 1.4; num1 - num2;}));
//    print_lfloat(({double num1 = 2.0;double num2 = 1.4; num1 * num2;}));
//    print_lfloat(({double num1 = 2.0;double num2 = 1.4; num1 / num2;}));
//    print_float(({float num1 = 2.0; 2.0 + 1.344 + 2.0004 / 2.9393 + 2323.0 * 4.2 / 1.0 * 3.0;}));
//    print_lfloat(({float a = 1.0; double b = 2.0; a + b;}));
//    assert(1,({long num = 4;long num1 = 2;(num + 3) % (1 + 1);}));
//    assert(1,({long num = 4;long num1 = 2;(num + 3) & (1 + 1);}));
    assert(111,({long num = 101;long num1 = 10;  num | num1;}));
    assert(15,({7 + 8 | 10 * 3 / 2 & 1 | 3;}));
    assert(2,({4 >> 1;}));
    assert(8,({4 << 1;}));
    assert(4,({8 >> 1 | 3 << 2 & 3  ;}));

    return 0;
}

//int testFuncAdd(int a,char b,short c,long d) {
//    return  a + b + c + d;
//}
