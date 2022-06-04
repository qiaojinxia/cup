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
extern void print_i(int);
void empty(){}

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

float sum_ff(float m,float x){
   return m + x;
}

int sum_i(int m,int x){
    return m + x;
}

int get_i(int n){
    if(n > 6) {
        return 7;
    }else{
        return 8;
    }
}

float get_float(float i){
    if(i > 2.0) {
        return 4.0;
    }else{
        return 8.0;
    }
}

int main() {
    static User a = {"xiaoli",{99,0,33},20,"bok222",92.0};
    assert_f(92.0,a.weight);
    User c ;
    c = mm(3);
    assert(27,c.age);
    assert_f(93.0,c.weight);
    c = mm1(3);
    assert(25,c.age);
    c = CmpUser(mm1(3),mm1(5));
    assert(48,c.age);
    assert_f(193.0,c.weight);
    float f1 = 1.0;
    float f2 = 2.0;
    assert_f(3.0,sum_ff(f1,f2));
    assert_f(4.0,get_float(3.0));
    assert_f(12.0,sum_ff(get_float(1.0),get_float(3.0)));
    int i1 = 1;
    int i2 = 2;
    assert(3,sum_i(i1,i2));
    assert(15,sum_i(get_i(7),get_i(4)));
    return 0;
}