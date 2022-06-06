//
// Created by qiaojinxia on 2022/6/4.
//
extern int  assert(int  excepted,int actual);
extern int  assert_f(float  excepted,float actual);
extern int  print_p(void *);
int main(){
    int a = 0;
    ++a;
    int b = (++a) + (++a);
    b += 1;
    b *= 3;
    b /= 3;
    assert(6,b);
    float f = 3.0;
    ++f;
    assert_f(4.0,f);
    static float f1 = 3.0;
    ++f1;
    assert_f(4.0,f1);
    int m1 = 3;
    int * m1p = &m1;
    ++m1p;
    int m2 = 5;
    assert(5,m2);
    static int sm1 = 3;
    ++sm1;
    assert(4,sm1);
    static * int sm1p = &sm1;
    ++sm1p;
    static int sm2 = 5;
    assert(1,sm1p - &sm1);
    int m[3] = {1,2,3};
    int  * p1 = &m;
    ++p1;
    assert(2,*p1);
    static int ms[3] = {1,2,3};
    static int * p2 = &ms;
    ++p2;
    assert(2,*p2);

    return 0;
}