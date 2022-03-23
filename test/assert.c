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