//
// Created by qiaojinxia on 2022/5/30.
//
extern int  assert(int  excepted,int actual);
extern int assert_f(float,float);
extern int assert_fd(double,double);
int main(){
    int m[8] = {1,4,5,6,7,8,9,0};
    assert(8,m[5]);
    float mf[10] = {1.0,4.0,5.0,6.0,7.0,8.0,9.0,0.0};
    assert_f(6.0,mf[3]);
    long m2[8] = {1,4,5,6,7,8,9,0};
    assert(8,m2[5]);
    double m3[8] = {23434.0,234.0123,3444.777,333.333,222.0,1423.0,12.123,4343.0};
    assert_fd(12.123,m3[6]);
    float m4[3] = {23434.0,234.0123,3444.777};
    assert_f(3444.777,m4[2]);
    double m5[3] = {23434.0,234.0123,3444.777};
    assert_fd((double)3444.777,m5[2]);
    long m6[3] = {1,2,3};
    assert(3,m6[2]);
    char m[12] = {5,2,8,1,23,45,32,61,54,65,123,43};
    assert(61,(int)m[7]);
    char m[6] = "aaaaaa";
    assert(97,(int)m[3]);
    return 0;
}