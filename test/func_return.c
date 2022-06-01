//
// Created by qiaojinxia on 2022/6/1.
//

typedef struct user{
    const char * name;
    int Score[3];
    int age ;
    char m[7] ;
    float weight ;
}User;
extern int  assert(int  excepted,int actual);
extern int  assert_f(float  excepted,float actual);
User mm(int m){
    User d = {"xiaoli",{99,0,33},20,"bok222",93.0};
    return d;
}

int main() {
    User a = {"xiaoli",{99,0,33},20,"bok222",92.0};
    User c ;
    c = mm(3);
    assert(20,c.age);
    assert_f(93.0,c.weight);
    return 0;
}