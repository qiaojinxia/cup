//
// Created by qiaojinxia on 2022/6/3.
//
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

    assert(3, ({ int i=2; ++i; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; ++*p; }));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; --*p; }));

    assert(2, ({ int i=2; i++; }));
    assert(2, ({ int i=2; i--; }));
    assert(3, ({ int i=2; i++; i; }));
    assert(1, ({ int i=2; i--; i; }));
    assert(1, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; *p++; }));
    assert(1, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; *p--; }));

    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; a[0]; }));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*(p--))--; a[1]; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p)--; a[2]; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p)--; p++; *p; }));

    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; a[0]; }));
    assert(0, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; a[1]; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; a[2]; }));
    assert(2, ({ int a[3]; a[0]=0; a[1]=1; a[2]=2; int *p=a+1; (*p++)--; *p; }));

    assert(0, !1);
    assert(0, !2);
    assert(1, !0);
    assert(1, !(char)0);
    assert(0, !(long)3);
    assert(4, sizeof(!(char)0));
    assert(4, sizeof(!(long)0));

    assert(-1, ~0);
    assert(0, ~-1);

    assert(5, 17%6);
    assert(5, ((long)17)%6);
    assert(2, ({ int i=10; i%=4; i; }));
    assert(2, ({ long i=10; i%=4; i; }));

    assert(0, 0&1);
    assert(1, 3&1);
    assert(3, 7&3);
    assert(10, -1&10);

    assert(1, 0|1);
    assert(0b10011, 0b10000|0b00011);

    assert(0, 0^0);
    assert(0, 0b1111^0b1111);
    assert(0b110100, 0b111000^0b001100);

    assert(2, ({ int i=6; i&=3; i; }));
    assert(7, ({ int i=6; i|=3; i; }));
    assert(10, ({ int i=15; i^=5; i; }));

    assert(1, 1<<0);
    assert(8, 1<<3);
    assert(10, 5<<1);
    assert(2, 5>>1);
    assert(-1, -1>>1);
    assert(1, ({ int i=1; i<<=0; i; }));
    assert(8, ({ int i=1; i<<=3; i; }));
    assert(10, ({ int i=5; i<<=1; i; }));
    assert(2, ({ int i=5; i>>=1; i; }));
    assert(-1, -1);
    assert(-1, ({ int i=-1; i; }));
    assert(-1, ({ int i=-1; i>>=1; i; }));

    assert(2, 0?1:2);
    assert(1, 1?1:2);
    assert(-1, 0?-2:-1);
    assert(-2, 1?-2:-1);
    assert(4, sizeof(0?1:2));
    assert(8, sizeof(0?(long)1:(long)2));
    assert(-1, 0?(long)-2:-1);
    assert(-1, 0?-2:(long)-1);
    assert(-2, 1?(long)-2:-1);
    assert(-2, 1?-2:(long)-1);

    1 ? -2 : (void)-1;

    assert(20, ({ int x; int *p=&x; p+20-p; }));
    assert(1, ({ int x; int *p=&x; p+20-p>0; }));
    assert(-20, ({ int x; int *p=&x; p-20-p; }));
    assert(1, ({ int x; int *p=&x; p-20-p<0; }));

    assert(15, (char *)0xffffffffffffffff - (char *)0xfffffffffffffff0);
    assert(-15, (char *)0xfffffffffffffff0 - (char *)0xffffffffffffffff);
    assert(1, (void *)0xffffffffffffffff > (void *)0);

    printf("OK\n");
    return 0;
}