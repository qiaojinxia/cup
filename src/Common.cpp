//
// Created by qiaojinxia on 2022/4/8.
//

#include "Common.h"
#include <stdio.h>
#include <stdarg.h> // va_start va_end
#include <string>

std::string  BDD::string_format(const char *format, ...)
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


 std::string BDD::convert_to_hex(long num){
    int i=0;
    std::string Hex="";
    do{
        switch(num%16)
        {
            case 1: Hex='1'+Hex;break;
            case 2: Hex='2'+Hex;break;
            case 3: Hex='3'+Hex;break;
            case 4: Hex='4'+Hex;break;
            case 5: Hex='5'+Hex;break;
            case 6: Hex='6'+Hex;break;
            case 7: Hex='7'+Hex;break;
            case 8: Hex='8'+Hex;break;
            case 9: Hex='9'+Hex;break;
            case 10: Hex='A'+Hex;break;
            case 11: Hex='B'+Hex;break;
            case 12: Hex='C'+Hex;break;
            case 13: Hex='D'+Hex;break;
            case 14: Hex='E'+Hex;break;
            case 15: Hex='F'+Hex;break;
            case 0: Hex='0'+Hex;break;
            default: break;
        }
        i++;
        num=num/16;
    }while(num);
    return "0x" + Hex;
}

