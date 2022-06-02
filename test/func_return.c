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
    User d = {"xiaoli",{99,0,33},27,"bok222",93.0};
    return d;
}

User CmpUser(User user1,User user2){
    User am = {"xiaoli",{99,0,33},28,"bok222",77.0};
    am.weight = user1.weight + user2.weight;
    am.age = user2.age + user1.age;
    return am;
}

User mm1(int m){
    User dm = {"xiaohei",{99,0,33},23,"bok222",100.0};
    User dm2 = {"xiaobai",{99,0,33},25,"bok2333",93.0};
    if(m >= 4){
        return dm;
    }else{
        return dm2;
    }
}

int main() {
    User a = {"xiaoli",{99,0,33},20,"bok222",92.0};
    User c ;
    c = mm(3);
    assert(27,c.age);
    assert_f(93.0,c.weight);
    c = mm1(3);
    assert(25,c.age);
    c = CmpUser(mm1(3),mm1(5));
    assert(48,c.age);
    assert_f(193.0,c.weight);
    return 0;
}