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
//    assert(4, sizeof(!(char)0));
    return 0;
}