//
// Created by qiaojinxia on 2022/4/8.
//
#include <string>
#ifndef BODDY_COMMON_H
#define BODDY_COMMON_H

namespace BDD{
    std::string string_format(const char* fmt, ...);
    std::string convert_to_hex(long num);
    char hexToDec(char hex);

    long hexToDec(std::basic_string_view<char> content, int length);

    long binToDec(std::basic_string_view<char> content, int length);

    void string_replace(std::string &origin, const std::string &search, const std::string &content);
}


#endif //BODDY_COMMON_H
