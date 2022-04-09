//
// Created by qiaojinxia on 2022/4/8.
//

#include "Common.h"
#include <stdio.h>
#include <stdarg.h> // va_start va_end
#include <string>

std::string BDD::string_format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int count = vsnprintf(NULL, 0, format, args);
    va_end(args);

    va_start(args, format);
    char *buff = (char *)malloc((count + 1) * sizeof(char));
    vsnprintf(buff, (count + 1), format, args);
    va_end(args);

    std::string str(buff, count);
    free(buff);
    return str;
}
