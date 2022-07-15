//
// Created by qiaojinxia on 2022/6/23.
//
extern int  assert(int  excepted,int actual);
int main(){
    assert(1, sizeof(char));
    assert(2, sizeof(short));
    assert(2, sizeof(short int));
    assert(2, sizeof(int short));
    assert(4, sizeof(int));
    assert(8, sizeof(long));
    assert(8, sizeof(long int));
    assert(8, sizeof(long int));
    assert(8, sizeof(char *));
    assert(8, sizeof(int *));
    assert(8, sizeof(long *));
    assert(8, sizeof(int **));
    assert(8, sizeof(int(*)[4]));
    assert(32, sizeof(int*[4]));
    assert(16, sizeof(int[4]));
    assert(48, sizeof(int[3][4]));
    assert(8, sizeof(struct {int a; int b;}));
    assert(4, sizeof(!(char)0));
    assert(4, sizeof(0?1:2));


    assert(8, sizeof(-10 + (long)5));
    assert(8, sizeof(-10 - (long)5));
    assert(8, sizeof(-10 * (long)5));
    assert(8, sizeof(-10 / (long)5));
    assert(8, sizeof((long)-10 + 5));
    assert(8, sizeof((long)-10 - 5));
    assert(8, sizeof((long)-10 * 5));
    assert(8, sizeof((long)-10 / 5));

    assert(1, ({ char i; sizeof(++i); }));
    assert(1, ({ char i; sizeof(i++); }));


    assert(8, sizeof(int(*)[10]));
    assert(8, sizeof(int(*)[][10]));

    assert(4, sizeof(struct { int x, y[]; }));


    assert(1, sizeof(char));
    assert(1, sizeof(signed char));
    assert(1, sizeof(signed char signed));
    assert(1, sizeof(unsigned char));
    assert(1, sizeof(unsigned char unsigned));

    assert(2, sizeof(short));
    assert(2, sizeof(int short));
    assert(2, sizeof(short int));
    assert(2, sizeof(signed short));
    assert(2, sizeof(int short signed));
    assert(2, sizeof(unsigned short));
    assert(2, sizeof(int short unsigned));

    assert(4, sizeof(int));
    assert(4, sizeof(signed int));
    assert(4, sizeof(signed));
    assert(4, sizeof(signed signed));
    assert(4, sizeof(unsigned int));
    assert(4, sizeof(unsigned));
    assert(4, sizeof(unsigned unsigned));

    assert(8, sizeof(long));
    assert(8, sizeof(signed long));
    assert(8, sizeof(signed long int));
    assert(8, sizeof(unsigned long));
    assert(8, sizeof(unsigned long int));

    assert(8, sizeof(long long));
    assert(8, sizeof(signed long long));
    assert(8, sizeof(signed long long int));
    assert(8, sizeof(unsigned long long));
    assert(8, sizeof(unsigned long long int));

    assert(1, sizeof((char)1));
    assert(2, sizeof((short)1));
    assert(4, sizeof((int)1));
    assert(8, sizeof((long)1));

    assert(4, sizeof((char)1 + (char)1));
    assert(4, sizeof((short)1 + (short)1));
    assert(4, sizeof(1?2:3));
    assert(4, sizeof(1?(short)2:(char)3));
    assert(8, sizeof(1?(long)2:(char)3));


    assert(1, sizeof(char) << 31 >> 31  );
    assert(1, sizeof(char) << 63 >> 63);


    assert(4, sizeof(float));
    assert(8, sizeof(double));

    assert(4, sizeof(1f+2));
    assert(8, sizeof(1.0 + 2));
    assert(4, sizeof(1f-2));
    assert(8, sizeof(1.0-2));
    assert(4, sizeof(1f*2));
    assert(8, sizeof(1.0*2));
    assert(4, sizeof(1f/2));
    assert(8, sizeof(1.0/2));
    return 0;
}