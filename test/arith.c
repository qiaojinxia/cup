//
// Created by qiaojinxia on 2022/6/3.
//
extern float assert_f(float  excepted,float actual);
extern int  assert(int  excepted,int actual);
int main() {
    assert(0, 0);
    assert(42, 42);
    assert(21, 5+20-4);
    assert(41,  12 + 34 - 5 );
    assert(47, 5+6*7);
    assert(15, 5*(9-6));
    assert(4, (3+5)/2);
    assert(10, -10+20);
    assert(10, - -10);
    assert(10, - - +10);

    assert(0, 0==1);
    assert(1, 42==42);
    assert(1, 0!=1);
    assert(0, 42!=42);

    assert(1, 0<1);
    assert(0, 1<1);
    assert(0, 2<1);
    assert(1, 0<=1);
    assert(1, 1<=1);
    assert(0, 2<=1);

    assert(1, 1>0);
    assert(0, 1>1);
    assert(0, 1>2);
    assert(1, 1>=0);
    assert(1, 1>=1);
    assert(0, 1>=2);

    assert(0, 1073741824 * 100 / 100);

    assert(7, ({ int i=2; i+=5; i; }));
    assert(7, ({ int i=2; i+=5; }));
    assert(3, ({ int i=5; i-=2; i; }));
    assert(3, ({ int i=5; i-=2; }));
    assert(6, ({ int i=3; i*=2; i; }));
    assert(6, ({ int i=3; i*=2; }));
    assert(3, ({ int i=6; i/=2; i; }));
    assert(3, ({ int i=6; i/=2; }));
    assert(1, ({ int i=7; i%=2; }));
    assert(16, ({ int i=4; i<<=2; }));
    assert(7, ({ int i=28; i>>=2; }));
    assert(-12, ({ int i=-10; i^=2; }));
    assert(6, ({ int i=2; i|=4; }));
    assert(2, ({ int i=7; i&=10; }));
    assert(3, ({ int i=2; ++i; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1;++*p; }));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; --*p; }));
    assert(2, ({ int i=2; i++; }));
    assert(2, ({ int i=2; i--; }));
    assert(3, ({ int i=2; i++; i; }));
    assert(1, ({ int i=2; i--; i; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; *p++;*p; }));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; *p--;*p; }));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--;a[1];}));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*(p--))--; a[1]; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p)--; a[2]; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p)--; p++; *p; }));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; a[0]; }));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; a[1]; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; a[2]; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; *p; }));
    assert_f(3.0, ({ float i=2.0; i++; i; }));
    assert_f(1.0, ({ float i=2.0; i--; i; }));
    return 0;
}
