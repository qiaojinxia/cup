//
// Created by qiaojinxia on 2022/6/16.
//
extern float assert_f(float  excepted,float actual);
extern int  assert(int  excepted,int actual);
int main(){
    assert_f(35, (float)(char)35);
    assert_f(35, (float)(short)35);
    assert_f(35, (float)(int)35);
    assert_f(35, (float)(long)35);
    assert_f(35, (float)(unsigned char)35);
    assert_f(35, (float)(unsigned short)35);
    assert_f(35, (float)(unsigned int)35);
    assert_f(35, (float)(unsigned long)35);

    assert(35, (double)(char)35);
    assert(35, (double)(short)35);
    assert(35, (double)(int)35);
    assert(35, (double)(long)35);
    assert(35, (double)(unsigned char)35);
    assert(35, (double)(unsigned short)35);
    assert(35, (double)(unsigned int)35);
    assert(35, (double)(unsigned long)35);

    assert(35, (char)(float)35);
    assert(35, (short)(float)35);
    assert(35, (int)(float)35);
    assert(35, (long)(float)35);
    assert(35, (unsigned char)(float)35);
    assert(35, (unsigned short)(float)35);
    assert(35, (unsigned int)(float)35);
    assert(35, (unsigned long)(float)35);

    assert(35, (char)(double)35);
    assert(35, (short)(double)35);
    assert(35, (int)(double)35);
    assert(35, (long)(double)35);
    assert(35, (unsigned char)(double)35);

    assert(-2147483648, (double)(unsigned long)(long)-1);

//
    assert(1, 2e3==2e3);
//    assert(0, 2e3==2e5);
    assert(1, 2.0==2);
    assert(0, 5.1<5);
    assert(0, 5.0<5);
    assert(1, 4.9<5);
    assert(0, 5.1<=5);
    assert(1, 5.0<=5);
    assert(1, 4.9<=5);

    assert(1, 2e3f==2e3);
//    assert(0, 2e3f==2e5);
    assert(1, 2.0f==2);
    assert(0, 5.1f<5);
    assert(0, 5.0f<5);
    assert(1, 4.9f<5);
    assert(0, 5.1f<=5);
    assert(1, 5.0f<=5);
    assert(1, 4.9f<=5);

    assert(6, 2.3+3.8);
    assert(-1, 2.3-3.8);
    assert(-3, -3.8);
    assert(13, 3.3*4);
    assert(2, 5.0/2);

    assert(6, 2.3f+3.8f);
    assert(6, 2.3f+3.8);
    assert(-1, 2.3f-3.8);
    assert(-3, -3.8f);
    assert(13, 3.3f*4);
    assert(2, 5.0f/2);

    return 0;
}