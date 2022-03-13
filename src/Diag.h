//
// Created by qiaojinxia on 2022/3/13.
//

#ifndef BODDY_DIAG_H
#define BODDY_DIAG_H

#include <string_view>

namespace BDD{
    void DiagE(std::string_view code,int line,int col,const char *fmt,...);
}

#endif //BODDY_DIAG_H
