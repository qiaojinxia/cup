//
// Created by qiaojinxia on 2022/5/27.
//
extern int  assert(int  excepted,int actual);
typedef struct user{
    const char * name;
    int age ;
}User;

int main(){
    static int a[3] = {1,2};
    a[1] = 3;
    assert(3,a[1]);
    return 0;
}