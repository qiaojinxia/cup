//
// Created by qiaojinxia on 2022/3/22.
//
#include <stdio.h>
#include <stdlib.h>
static int codeId = 0;
void assert(int  excepted,int actual){
    if(excepted == actual){
        printf("code(%d) => %d\n",codeId ++ ,actual);
    }else{
        printf("code(%d) expected %d,but got %d\n",codeId++,excepted,actual);
        exit(-1);
    }
}

void assert_f(float  excepted,float actual){
    if(excepted  - actual <= 0.000001 ){
        printf("code(%d) => %f\n",codeId ++ ,actual);
    }else{
        printf("code(%d) expected %f,but got %f\n",codeId++,excepted,actual);
        exit(-1);
    }
}

void assert_fd(float  excepted,double actual){
    if(excepted  - actual <= 0.000001 &&  excepted  - actual >=  -0.000001){
        printf("code(%d) => %f\n",codeId ++ ,actual);
    }else{
        printf("code(%d) expected %f,but got %f\n",codeId++,excepted,actual);
        exit(-1);
    }
}


void print_float(double a){
    printf("%f\n",a);
}