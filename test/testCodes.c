#宏 吧啦啦能量沙罗沙罗小魔仙全身变 enum 斗者临兵皆阵列前行
#宏 吧啦啦能量沙罗沙罗小魔仙全身变 struct 道生一,一生二,二生三,三生万物
#宏 吧啦啦能量沙罗沙罗小魔仙全身变 typedef 无名天地之始,有名万物之母
#宏 吧啦啦能量沙罗沙罗小魔仙全身变 extern 天下万物生于有,有生于无
天下万物生于有,有生于无 int  assert(int  excepted,int actual);
天下万物生于有,有生于无 int  assert_f(float  excepted,float actual);
无名天地之始,有名万物之母 道生一,一生二,二生三,三生万物 user{
    char *  name;
    int  age;
    int  sex;
    int height;
    float width;
} User ;
int UserStructAdd(User a,User * b){
    b -> age = 30;
    a.age = 20;
    return a.age + b -> age ;
}
User UserStructReturn(User a,User * b){
    User xiaoli;
    b->height = 30;
    if(a.height >= 190){ xiaoli.height = 99;}else{xiaoli.height = 77;}
    return xiaoli;
}
int testFuncCallWithStruct(){
    斗者临兵皆阵列前行 Sex {MALE = 1,FEMALE = 0};
    User xiaozhang = {"zhangsan",10,MALE,190,140.0};
    assert(50,UserStructAdd(xiaozhang,&xiaozhang));
    User lisi = UserStructReturn(xiaozhang,&xiaozhang);
    assert(190,xiaozhang.height);
    assert(30,xiaozhang.age);
    assert(99,lisi.height);
}
//
//float testFloatAdd(float a,float b){
//    float x = 0.0;
//    for (int i = 0; i < 10 ; i++) {
//       x = x + 10.0;
//    }
//    return a + b + x;
//}
//int testIntAdd(int a,int b){
//    return a + b;
//}
//
//int testIf(){
//    float a = 300.0;float b = 400.0;
//    int c = 100; int d = 200;
//    if (b >= 399.0 && a > 301.0 || b < 10){
//        a = a + 1.0;
//    }else{
//        a = a - 3.0;
//    }
//
//    assert(0,(int)(a > 300.0));
//    assert_f(297.0,a);
//    int e = 0;
//    int f = 1;
//    int * g = &f;
//    if(e || *g && e){
//        assert(1,0);
//    }
//    assert_f(797.0,testFloatAdd(a,b));
//    assert(300,testIntAdd(c,d));
//}
//


int testCode(){
//    testEnum();
//    testSwitch();
//    testConstant();-
    testFuncCallWithStruct();
//    testIf();
    return 0;
}

//int testConstant(){
//    int  b = 3;
//    int * const  pointer=3;
//    pointer = &b;
//}



//int testSwitch(){
//    int a = 1;
//    switch (a) {case 1:a = a + 2;case 2:a = a + 5;break;case 3:a = a + 1;break;default:a = 5;}
//    assert(3,a);
//}
//int testEnum(){
//    float m[3] = {3.0,1.0,2.0};
//    print_float(m[2]);
//    float x = 5.0;
//    print_float(x);
//    int b = 7;
//    assert(7,b);
//    char * my = "hello,world!\n";
//    printf("%s",my);
//    float m1 = m[2];
//    assert_f(m[1] + 1.0 ,2.0);
//    assert_f(m[2] - m[1] ,1.0);
//    int x1 = 3;
//    int x2 = 2;
//    assert(1,x1-x2);
//    double d1 = 4.0;
//    double d2 = 5.0;
//    assert_fd((double)-1.0 ,d1 - d2);
//    double d3 = d1 * d2;
//    int x3 = -4;
//      char u1 = 123;
//      int m = 3;
//    assert_fd((double)20.0 ,d3);
//    assert(-8 ,x2 * x3);
//    assert_u((unsigned long)4262462462462464646 ,u1 * x2);
//    unsigned  long x6 = 18446744073709551615;
//    int m = 2;
//    assert_u((unsigned  long)9223372036854775807,x6/m);
//    long m = c;
//    assert(1,({2147483647 + 2147483647 + 2;}));
//    float x = 1.3243244;
//    double max_d = 0;
//    assert_f(0.6621622,({x/2;}));
//    assert_fd((double)-1.0,({max_d-1;}));
//    long m1 = 985;

//    typedef struct User
//    {
//        char *  name;
//        int  age;
//        int  sex;
//        int height;
//        int width;
//    } user ;
//    user a = {"caoMaoBoy_a",10,1,190,140};
//    user b = {"caoMaoBoy_b",11,0,190,140};
//    typedef int zhengShu;
//    zhengShu i = 0;
//    zhengShu mi = 11;
//    zhengShu ma[3] ={1,2,3};
//    float f22 = 1.0;
//    float f23 = 1.0;
//    int mmmx = f22 != f23;
//    zhengShu b1 = 0b00001111;
//    zhengShu b2 = 0b11110001;
//    zhengShu b3 = b1 & b2;
//    zhengShu b5 = 0b00000010;
//    zhengShu b6 = b2 | b5;
//    zhengShu *p1 = &i;
//    zhengShu *pi = &ma;
//    struct User *pa = &a;
//    printf("Hello, %s!\n",(pa+1)->name);
//    float m1[3] = {3.0,1.0,2.0};
//    float * cm = &m1[3];
//    float * dm = &m1[2];
//    enum Bool {true = 1,false = 0};
//    int m = true;
//    assert(3,({(i++)+(i++)+(i++);}));
//    assert(0,mmmx);
//    assert(12,b1 ^ 3);
//    assert(240,b1 << 4);
//    assert(15,b2 >> 4);
//    assert(1,b3);
//    assert(243,b6);
//    assert(11,(pa +1)->age);
//    assert(11,*(p1 +1));
//    assert(2,*(pi +1));
//    assert(1,cm-dm);
//    assert(-2,~m);
//     int m = 3;
//     int z = 4;
//     int  y = m > z ? m * 3:(m-1) * 2;
//     assert(4,y);
//     int mx = !y;
//     assert(0,mx);
//}

