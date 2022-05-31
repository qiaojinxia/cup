//
// Created by qiaojinxia on 2022/5/27.
//
extern int  assert(int  excepted,int actual);
extern int  assert_f(float  excepted,float actual);
typedef struct user{
    const char * name;
    int Score[3];
    int age ;
    char m[7] ;
    float weight ;
}User;

int main(){
    static int a[3] = {1,2};
    a[1] = 3;
    assert(3,a[1]);
    User a = {"xiaoli",{99,67,33},20,"book22!",92.0};
    assert(67,a.Score[1]);
    assert(99,*a.Score);
    assert(20,a.age);
    assert_f(92.0,a.weight);
    static User b = {"akai",{99,100,99},56,"book!",122.0};
    assert(56,b.age);
    assert_f(122.0,b.weight);
    return 0;
}