//
// Created by qiaojinxia on 2022/3/13.
//

#include <iostream>
#include "Diag.h"

namespace BDD{
    void DiagE(std::string_view code,int line,int col,const char *fmt,...){
        va_list ap;
        va_start(ap,fmt);
        std::cerr << code << std::endl;
        fprintf(stderr,"%*s^",col ,"");
        vfprintf(stderr,fmt,ap);
        fprintf(stderr,"\n");
        va_end(ap);
        exit(0);
    }
}
