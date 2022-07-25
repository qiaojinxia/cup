
int g1, g2[4];
static int g3 = 3;
extern int  assert(int  excepted,int actual);

int main() {
    assert(3, ({ int a; a=3; a; }));
    assert(3, ({ int a=3; a; }));
    assert(8, ({ int a=3; int z=5; a+z; }));

    assert(3, ({ int a=3; a; }));
    assert(8, ({ int a=3; int z=5; a+z; }));
    assert(9, ({ int a; int b;int c; a=b=c = 3; a+b+c; }));
    assert(3, ({ int foo=3; foo; }));
    assert(8, ({ int foo123=3; int bar=5; foo123+bar; }));

    assert(4, ({ int x; sizeof(x); }));
    assert(4, ({ int x; sizeof x; }));
    assert(8, ({ int *x; sizeof(x); }));
    assert(16, ({ int x[4]; sizeof(x); }));
    assert(48, ({ int x[3][4]; sizeof(x); }));
    assert(16, ({ int x[3][4]; sizeof(*x); }));
    assert(4, ({ int x[3][4]; sizeof(**x); }));
    assert(5, ({ int x[3][4]; sizeof(**x) + 1; }));
    assert(5, ({ int x[3][4]; sizeof **x + 1; }));
    assert(4, ({ int x[3][4]; sizeof(**x + 1); }));
    assert(4, ({ int x=1; sizeof(x=2); }));
    assert(1, ({ int x=1; sizeof(x=2); x; }));

    assert(0, g1);
    assert(3, ({ g1=3; g1; }));
    assert(0, ({ g2[0]=0; g2[1]=1; g2[2]=2; g2[3]=3; g2[0]; }));
    assert(1, ({ g2[0]=0; g2[1]=1; g2[2]=2; g2[3]=3; g2[1]; }));
    assert(2, ({ g2[0]=0; g2[1]=1; g2[2]=2; g2[3]=3; g2[2]; }));
    assert(3, ({ g2[0]=0; g2[1]=1; g2[2]=2; g2[3]=3; g2[3]; }));

    assert(4, sizeof(g1));
    assert(16, sizeof(g2));

    assert(1, ({ char x=1; x; }));
    assert(1, ({ char x=1; char y=2; x; }));
    assert(2, ({ char x=1; char y=2; y; }));

    assert(1, ({ char x; sizeof(x); }));
    assert(10, ({ char x[10]; sizeof(x); }));

    assert(2, ({ int x=2; { int x=3; } x; }));
    assert(2, ({ int x=2; { int x=3; } int y=4; x; }));
    assert(3, ({ int x=2; { x=3; } x; }));

    assert(-1, ({ int x; int y; char z; char *a=&y; char *b=&z; b-a; }));
    assert(-7, ({ int x; char y; int z; char *a=&y; char *b=&z; b-a; }));

    assert(8, ({ long x; sizeof(x); }));
    assert(2, ({ short x; sizeof(x); }));

    assert(24, ({ char *x[3]; sizeof(x); }));
    assert(8, ({ char (*x)[3]; sizeof(x); }));
    assert(1, ({ char (x); sizeof(x); }));
    assert(3, ({ char (x)[3]; sizeof(x); }));
    assert(12, ({ char (x[3])[4]; sizeof(x); }));
    assert(4, ({ char (x[3])[4]; sizeof(x[0]); }));
    assert(3, ({ char *x[3]; char y; x[0]=&y; y=3;x[0][0]; }));
    assert(3, ({ char x[3]; char (*y)[3]=x; y[1]-y[0]; }));
    assert(3, ({ char x[3]; char (*y)[3]=x; *(y+1)-*(y+0); }));
    assert(4, ({ char x[3]; char (*y)[3]=x; y[0][0]=4; y[0][0]; }));

    { void *x; }
    //todo y[i][j]=4;
    assert(3, g3);

  return 0;
}
