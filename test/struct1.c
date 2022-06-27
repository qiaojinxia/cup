//
// Created by qiaojinxia on 2022/6/26.
//

extern int  assert(int  excepted,int actual);
int main(){

    assert(1, ({ struct {int a; int b;} x; x.a=1; x.b=2; x.a; }));
    assert(2, ({ struct {int a; int b;} x; x.a=1; x.b=2; x.b; }));
    assert(1, ({ struct {char a; int b; char c;} x; x.a=1; x.b=2; x.c=3; x.a; }));
    assert(2, ({ struct {char a; int b; char c;} x; x.b=1; x.b=2; x.c=3; x.b; }));
    assert(3, ({ struct {char a; int b; char c;} x; x.a=1; x.b=2; x.c=3; x.c; }));

    assert(0, ({ struct {char a; char b;} x[3]; char *p=x; p[0]=0; x[0].a; }));
    assert(1, ({ struct {char a; char b;} x[3]; char *p=x; p[1]=1; x[0].b; }));
    assert(2, ({ struct {char a; char b;} x[3]; char *p=x; p[2]=2; x[1].a; }));
    assert(3, ({ struct {char a; char b;} x[3]; char *p=x; p[3]=3; x[1].b; }));

    assert(6, ({ struct {char a[3]; char b[5];} x; char *p=&x; x.a[0]=6; p[0]; }));
    assert(7, ({ struct {char a[3]; char b[5];} x; char *p=&x; x.b[0]=7; p[3]; }));

    assert(6, ({ struct { struct { char b; } a; } x; x.a.b=6; x.a.b; }));

    assert(4, ({ struct {int a;} x; sizeof(x); }));
    assert(8, ({ struct {int a; int b;} x; sizeof(x); }));
    assert(8, ({ struct {int a, b;} x; sizeof(x); }));
    assert(12, ({ struct {int a[3];} x; sizeof(x); }));
    assert(16, ({ struct {int a;} x[4]; sizeof(x); }));
    assert(24, ({ struct {int a[3];} x[2]; sizeof(x); }));
    assert(2, ({ struct {char a; char b;} x; sizeof(x); }));
    assert(0, ({ struct {} x; sizeof(x); }));
    assert(8, ({ struct {char a; int b;} x; sizeof(x); }));
    assert(8, ({ struct {int a; char b;} x; sizeof(x); }));
//
//    assert(8, ({ struct t {int a; int b;} x; struct t y; sizeof(y); }));
//    assert(8, ({ struct t {int a; int b;}; struct t y; sizeof(y); }));
//    assert(2, ({ struct t {char a[2];}; { struct t {char a[4];}; } struct t y; sizeof(y); }));
//    assert(3, ({ struct t {int x;}; int t = 1; struct t y; y.x=2; t+y.x; }));
//
//    assert(3, ({ struct t {char a;} x; struct t *y = &x; x.a=3; y->a; }));
//    assert(3, ({ struct t {char a;} x; struct t *y = &x; y->a=3; x.a; }));
//
//    assert(3, ({ struct {int a,b;} x,y; x.a=3; y=x; y.a; }));
//    assert(7, ({ struct t {int a,b;}; struct t x; x.a=7; struct t y; struct t *z=&y; *z=x; y.a; }));
//    assert(7, ({ struct t {int a,b;}; struct t x; x.a=7; struct t y, *p=&x, *q=&y; *q=*p; y.a; }));
//    assert(5, ({ struct t {char a, b;} x, y; x.a=5; y=x; y.a; }));
//
//    assert(3, ({ struct {int a,b;} x,y; x.a=3; y=x; y.a; }));
//    assert(7, ({ struct t {int a,b;}; struct t x; x.a=7; struct t y; struct t *z=&y; *z=x; y.a; }));
//    assert(7, ({ struct t {int a,b;}; struct t x; x.a=7; struct t y, *p=&x, *q=&y; *q=*p; y.a; }));
//    assert(5, ({ struct t {char a, b;} x, y; x.a=5; y=x; y.a; }));
//
//    assert(8, ({ struct t {int a; int b;} x; struct t y; sizeof(y); }));
//    assert(8, ({ struct t {int a; int b;}; struct t y; sizeof(y); }));
//
//    assert(16, ({ struct {char a; long b;} x; sizeof(x); }));
//    assert(4, ({ struct {char a; short b;} x; sizeof(x); }));
//
//    assert(8, ({ struct foo *bar; sizeof(bar); }));
//    assert(4, ({ struct T *foo; struct T {int x;}; sizeof(struct T); }));
//    assert(1, ({ struct T { struct T *next; int x; } a; struct T b; b.x=1; a.next=&b; a.next->x; }));
//    assert(4, ({ typedef struct T T; struct T { int x; }; sizeof(T); }));

    return 0;
}