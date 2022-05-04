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

long BDD::hexToDec(std::basic_string_view<char> content, int length){
    unsigned long result = 0;
    for (int i = 0; i < length; i++) {
        result += hexToDec(content[i]) * (1 << (4 * (length - i -1)));
    }
    return result;
}

long BDD::binToDec(std::basic_string_view<char> content, int length){
    unsigned long result = 0;
    for (int i = 0; i < length; i++) {
        result += (content[i] - '0') << (length - i -1);
    }
    return result;
}


char BDD::hexToDec(char hex){
    if (hex >= '0' && hex <= '9') return hex-'0';
    if (hex >= 'A'  && hex <= 'F') return hex-'A'+10;
    if (hex  >= 'a' && hex <= 'f') return hex-'a'+10;
    return hex=0;
}

void BDD::string_replace(std::string &origin,const std::string &search,const std::string&content)
{
    std::string::size_type pos=0;
    std::string::size_type a= search.size();
    std::string::size_type b= content.size();
    while((pos=origin.find(search,pos))!=std::string::npos)
    {
        origin.replace(pos,a,content);
        pos+=b;
    }
}

bool BDD::is_contains_str(std::string str,std::string contains_str) {
    std::string::size_type idx = str.find(contains_str);
    if (idx != std::string::npos) {
        return true;
    }
    return false;
}


BDD::IteratorNode BDD::Str2IntArrayIterator::next() {
    auto maxSize =  Cursor + 8;
    if (maxSize > Content.size()){
        maxSize =  Content.size()  ;
    }
    auto numSize = maxSize - Cursor  ;
    int zero = 0;
    if(numSize > 4){
        zero = 8 - numSize;
    }else if(numSize > 2){
        zero = 4 - numSize ;
    }

    auto curVal = std::string(Content.substr(Cursor,maxSize));
    unsigned long strValue = 0;
    for (int i = 0; i < curVal.size(); i++) {
        strValue += (unsigned long)curVal[curVal.size()-i-1] << ( 8 * (curVal.size() - i -1));
    }
    Cursor += maxSize;
    return BDD::IteratorNode(strValue, numSize + zero);
}

bool BDD::Str2IntArrayIterator::has_next() {
    return Cursor < Content.size() ;
}
