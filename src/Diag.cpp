//
// Created by qiaojinxia on 2022/3/13.
//

#include <iostream>
#include "Diag.h"
#include "AstNode.h"
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

    void DiagLoc(std::string_view code,SourceLocation location,const char *fmt,...){
        va_list ap;
        va_start(ap,fmt);
        std::cerr << code << std::endl;
        fprintf(stderr,"%*s^",location.Col ,"");
        vfprintf(stderr,fmt,ap);
        fprintf(stderr,"\n");
        va_end(ap);
        exit(0);
    }
}
