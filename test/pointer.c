//
// Created by qiaojinxia on 2022/6/18.
//
extern int  assert(int  excepted,int actual);
extern int  assert_f(float  excepted,float actual);
int main(){
    assert(3, ({ int x=3; *&x; }));
    assert(3, ({ int x=3; int *y=&x; int **z=&y; **z; }));
    assert(5, ({ int x=3; int y=5; *(&x-1); }));
    assert(3, ({ int x=3; int y=5; *(&y+1); }));

    assert(3, ({ int x=3; int y=5; *(&y-(-1)); }));
    assert(5, ({ int x=3; int *y=&x; *y=5; x; }));
    assert(7, ({ int x=3; int y=5; *(&x-1)=7; y; }));
    assert(7, ({ int x=3; int y=5; *(&y+2-1)=7; x; }));
    assert(5, ({ int x=3; (&x+2)-&x + 3; }));
    assert(8, ({ int x, y; x=3; y=5; x+y; }));
//    assert(8, ({ int x=3, y=5; x+y; }));

    assert(3, ({ int x[2]; int *y=&x; *y=3; *x; }));
//
    assert(3, ({ int x[3]; *x=3; *(x+1)=4; *(x+2)=5; *x; }));
    assert(4, ({ int x[3]; *x=3; *(x+1)=4; *(x+2)=5; *(x+1); }));
    assert(5, ({ int x[3]; *x=3; *(x+1)=4; *(x+2)=5; *(x+2); }));
//
    assert(0, ({ int x[2][3]; int *y=x; *y=0; **x; }));
    assert(1, ({ int x[2][3]; int *y=x; *(y+1)=1; *(*x+1); }));
    assert(2, ({ int x[2][3]; int *y=x; *(y+2)=2; *(*x+2); }));
    assert(3, ({ int x[2][3]; int *y=x; *(y+3)=3; **(x+1); }));
    assert(4, ({ int x[2][3]; int *y=x; *(y+4)=4; *(*(x+1)+1); }));
    assert(5, ({ int x[2][3]; int *y=x; *(y+5)=5; *(*(x+1)+2); }));
//
    assert(3, ({ int x[3]; *x=3; x[1]=4; x[2]=5; *x; }));
    assert(4, ({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+1); }));
    assert(5, ({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+2); }));
    assert(5, ({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+2); }));
    assert(5, ({ int x[3]; *x=3; x[1]=4; 2[x]=5; *(x+2); }));

    assert(0, ({ int x[2][3]; int *y=x; y[0]=0; x[0][0]; }));
    assert(1, ({ int x[2][3]; int *y=x; y[1]=1; x[0][1]; }));
    assert(2, ({ int x[2][3]; int *y=x; y[2]=2; x[0][2]; }));
    assert(3, ({ int x[2][3]; int *y=x; y[3]=3; x[1][0]; }));
    assert(4, ({ int x[2][3]; int *y=x; y[4]=4; x[1][1]; }));
    assert(5, ({ int x[2][3]; int *y=x; y[5]=5; x[1][2]; }));
    return 0;
}