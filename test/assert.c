//
// Created by qiaojinxia on 2022/3/22.
//
#include <stdio.h>
#include <stdlib.h>
static int codeId = 0;
void assert(int  excepted,int actual){
    if(excepted == actual){
        printf("code test pass (%d) => %d\n",codeId ++ ,actual);
    }else{
        printf("code test pass (%d) expected %d,but got %d\n",codeId++,excepted,actual);
        exit(-1);
    }
}

void assert_u(long  excepted, long actual){
    if(excepted == actual){
        printf("code test pass (%d) => %lu\n",codeId ++ ,actual);
    }else{
        printf("code test pass (%d) expected %ld,but got %lu\n",codeId++,excepted,actual);
        exit(-1);
    }
}


void assert_f(float  excepted,float actual){
    if(excepted  - actual <= 0.000001 && excepted  - actual >= -0.000001 ){
        printf("code test pass (%d) => %f\n",codeId ++ ,actual);
    }else{
        printf("code test  (%d) expected %f,but got %f\n",codeId++,excepted,actual);
        exit(-1);
    }
}

void assert_char(char  excepted,char actual){
    if(excepted  == actual ){
        printf("code test pass (%d) => %d\n",codeId ++ ,actual);
    }else{
        printf("code test pass (%d) expected %c,but got %d\n",codeId++,excepted,actual);
        exit(-1);
    }
}


void assert_fd(double  excepted,double actual){
    if(excepted  - actual <= 0.000001 &&  excepted  - actual >=  -0.000001){
        printf("code test pass (%d) => %lf\n",codeId ++ ,actual);
    }else{
        printf("code test pass (%d) expected %lf,but got %lf\n",codeId++,excepted,actual);
        exit(-1);
    }
}
void print_s(char * a){
    printf("output:%s\n",a);
}

void print_i(int a){
    printf("%d\n",a);
}


int put(char a){
    printf("%c",a);
    return 0;
}


int printfint(int a){
    printf("output:%d\n",a);
    return 0;
}

int printft(float a){
    printf("output:%f\n",a);
    return 0;
}

void print_float(float a){
    printf("output:%f\n",a);
}

void print_p(void * a){
    printf("output:%p\n",a);
}

void print_lfloat(double a){
    printf("output:%lf\n",a);
}