//
// Created by qiaojinxia on 2022/4/8.
//
#include <string>
#ifndef BODDY_COMMON_H
#define BODDY_COMMON_H

namespace BDD{
    class IteratorNode{
    public:
        unsigned long Value;
        int Size;
        IteratorNode(unsigned long v,int s):Value(v),Size(s){};
    };
    class Str2IntArrayIterator{
    private:
        int Cursor = {0};
        std::string_view Content;
    public:
        Str2IntArrayIterator(std::string_view content):Content(content){
            Content = Content.substr(1,content.size()-2);
        };
        IteratorNode next();
        bool has_next();
    };

    std::vector<std::string> split_str(const std::string& s, char delimiter);

    std::string string_format(const char* fmt, ...);
    std::string convert_to_hex(long num);
    char hexToDec(char hex);

    long hexToDec(std::basic_string_view<char> content, int length);

    long binToDec(std::basic_string_view<char> content, int length);

    void string_replace(std::string &origin, const std::string &search, const std::string &content);

    bool is_contains_str(std::string str, std::string contains_str);
}


#endif //BODDY_COMMON_H
