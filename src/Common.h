//
// Created by qiaojinxia on 2022/4/8.
//
#include <string>
#include "Type.h"
#include "AstNode.h"

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
    static std::unordered_map< std::string, std::string> CastMap{};
    std::vector<std::string> split_str(const std::string& s, char delimiter);

    std::string string_format(const char* fmt, ...);
    std::string convert_to_hex(long num);
    char hexToDec(char hex);

    long hexToDec(std::basic_string_view<char> content, int length);

    long binToDec(std::basic_string_view<char> content, int length);

    void string_replace(std::string &origin, const std::string &search, const std::string &content);

    bool is_contains_str(std::string str, std::string contains_str);

    std::string read_file(std::string path);


    const std::string GetMoveCode(std::shared_ptr<Type> type);
    const std::string GetMoveCode2(std::shared_ptr<Type>  type);

    const std::string GetIDivCode(std::shared_ptr<Type> type);

    const std::string GetRax(std::shared_ptr<Type> type);
    const std::string GetRcx(std::shared_ptr<Type> type);
    const std::string GetRcx(int size);
    const std::string GetRax(int size);
    const std::string GetAdd(std::shared_ptr<Type> type);

    std::string GetCastCode(std::string fromTo);

    const std::string GetRdi(std::shared_ptr<Type> sharedPtr);

    const std::string GetDivCode(std::shared_ptr<Type> type);


    const std::string GetMoveCode2(int size);

    const std::string GetMinus(std::shared_ptr<Type> type);

    const std::string GetMul(std::shared_ptr<Type> type);

    const std::string GetDiv(std::shared_ptr<Type> type);

    const std::string GetRdx(std::shared_ptr<Type> type);

    const std::string GetSet(BinaryOperator op);

    std::string GetReverseJmp(BinaryOperator anOperator);

    const std::string GetMoveCode(int size);

    const std::string RepeatN(std::string a, int n);
    std::string GetStoreCode(int size);
}


#endif //BODDY_COMMON_H
