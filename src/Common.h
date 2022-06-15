//
// Created by qiaojinxia on 2022/4/8.
//
#include <string>
#include "Type.h"
#include "AstNode.h"

#ifndef BODDY_COMMON_H
#define BODDY_COMMON_H

namespace BDD{
    using TypeCastMap = std::unordered_map< std::string, std::string>;
    static const TypeCastMap castMap {
            {"i8->i32","movsx %al, %eax"},
            {"u8->i32","movzbl %al, %eax"},
            {"bool->i32","movzbl %al, %eax"},
            {"u8->u32", "movzbl %al, %eax"},

            {"i16->i32","movswl %ax, %eax"},
            {"u16->u32","movzwl %ax, %eax"},
            {"i32->f32","cvtsi2ssl %eax, %xmm0"},
            {"i32->i64","movsx %eax, %rax"},

            {"i16->u32","movzwl %ax, %eax"},

            {"u32->u64","NULL"},


            {"i32->i8", "movsx %al, %rax"},
            {"i32->u8","NULL"},

            {"i32->i16", "movsx %ax, %rax"},
            {"i32->u32", "NULL"},

            {"i32->u64", "NULL"},
            {"u64->i32", "NULL"},
            {"u32->i32", "NULL"},
            {"u16->i32", "movzwl %ax, %eax"},

            {"u64->i64", "NULL"},
            {"i64->u64", "NULL"},

            {"i32->f64","cvtsi2sdl %eax, %xmm0"},

            {"u32->f32","mov %eax, %eax; cvtsi2ssq %rax, %xmm0"},
            {"u32->i64","NULL"},
            {"u32->f64","movzx %eax, %rax; cvtsi2sdq %rax, %xmm0"},

            {"i64->f32","cvtsi2ssq %rax, %xmm0"},
            {"i64->f64","cvtsi2sdq %rax, %xmm0"},
            {"i64->i32","NULL"},
            {"u64->f32","cvtsi2ssq %rax, %xmm0"},

            {"u64->f64",
                    "test %rax,%rax; js 1f; pxor %xmm0,%xmm0; cvtsi2sd %rax,%xmm0; jmp 2f; "
                    "1: mov %rax,%rdi; and $1,%eax; pxor %xmm0,%xmm0; shr %rdi; "
                    "or %rax,%rdi; cvtsi2sd %rdi,%xmm0; addsd %xmm0,%xmm0; 2:"},

            {"f32->i8","cvttss2sil %xmm0, %eax; movsbl %al, %eax"},
            {"f32->u8","cvttss2sil %xmm0, %eax; movzbl %al, %eax"},
            {"f32->i16","cvttss2sil %xmm0, %eax; movswl %ax, %eax"},
            {"f32->u16","cvttss2sil %xmm0, %eax; movzwl %ax, %eax"},
            {"f32->i32","cvttss2sil %xmm0, %eax"},
            {"f32->u32","cvttss2siq %xmm0, %rax"},
            {"f32->i64","cvttss2siq %xmm0, %rax"},//
            {"f32->u64","cvttss2siq %xmm0, %rax"},
            {"f32->f64","cvtss2sd %xmm0, %xmm0"},

            {"f64->i8","cvttsd2sil %xmm0, %eax; movsbl %al, %eax"},
            {"f64->u8","cvttsd2sil %xmm0, %eax; movzbl %al, %eax"},
            {"f64->i16","cvttsd2sil %xmm0, %eax; movswl %ax, %eax"},
            {"f64->u16","cvttsd2sil %xmm0, %eax; movzwl %ax, %eax"},
            {"f64->i32","cvttsd2sil %xmm0, %eax"},//
            {"f64->u32","cvttsd2siq %xmm0, %rax"},
            {"f64->f32","cvtsd2ss %xmm0, %xmm0"}, //
            {"f64->i64","cvttsd2siq %xmm0, %rax"}, //
            {"f64->u64","cvttsd2siq %xmm0, %rax"},
    };
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
    const std::string GetUnsignedSet(BinaryOperator op);

    std::string GetReverseJmp(BinaryOperator anOperator);

    const std::string GetMoveCode(int size);
    const std::string GetSuffix(int size);
    const std::string RepeatN(std::string a, int n);
    std::string GetStoreCode(int size);

    std::shared_ptr<BDD::AstNode> CastNodeType(std::shared_ptr<Type> srcType, std::shared_ptr<Type> destType,
                                               std::shared_ptr<BDD::AstNode> destNode);
}


#endif //BODDY_COMMON_H
