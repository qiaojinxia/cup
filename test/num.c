//
// Created by qiaojinxia on 2022/6/7.
//

extern int  assert_f(float  excepted,float actual);
int main(){
    float m = 3.0;
    float x = 4.0;
    assert_f(7.0,m+x);
    return 0;
}