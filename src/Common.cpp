//
// Created by qiaojinxia on 2022/4/8.
//

#include "Common.h"
#include<vector>
#include <sstream>      // std::istringstream
#include <string>
#include <iostream>
#include <fstream>
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
    std::string ct(content);
    if (is_contains_str(ct,"0x")){
        string_replace(ct,"0x","");
    }
    unsigned long result = 0;
    length = ct.size();
    for (int i = 0; i < length; i++) {
        result += hexToDec(ct[i]) * ((unsigned long)1 << (4 * (length - i -1)));
    }
    return result;
}

long BDD::binToDec(std::basic_string_view<char> content, int length){
    std::string ct(content);
    int start = 0;
    if (is_contains_str(ct,"0b")){
        start = 2;
    }
    unsigned long result = 0;
    for (int i = start; i < length; i++) {
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




std::vector<std::string>  BDD::split_str(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream  tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
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


std::string  BDD::read_file(std::string path){
    auto currentPath = std::filesystem::current_path();
    std::ifstream t;
    int length;
    t.open(path);      // open input file
    t.seekg(0, std::ios::end);    // go to the end
    length = t.tellg();           // report location (this is the length)
    t.seekg(0, std::ios::beg);    // go back to the beginning
    char *buffer = new char[length];    // allocate memory for a buffer of appropriate dimension
    t.read(buffer, length);       // read the whole file into the buffer
    t.close();
    std::string inputStr = buffer;
    return inputStr;
}

std::string BDD::GetStoreCode(int size) {
    switch (size){
        case 1:
            return ".byte";
            break;
        case 2:
            return ".short";
            break;
        case 4:
            return ".long";
            break;
        case 8:
            return ".quad";
            break;
        default:
            assert(0);
    }
}

std::string BDD::GetReverseJmp(BinaryOperator anOperator) {
    switch (anOperator) {
        case BinaryOperator::Greater:
            return "jle";
        case BinaryOperator::FloatPointGreater:
            return "jbe";
        case BinaryOperator::FloatPointLesser:
            return "jae";
        case BinaryOperator::GreaterEqual:
            return "jl";
        case BinaryOperator::FloatPointGreaterEqual:
            return "jb";
        case BinaryOperator::FloatPointLesserEqual:
            return "ja";
        case BinaryOperator::Lesser:
            return "jge";
        case BinaryOperator::LesserEqual:
            return "jg";
        case BinaryOperator::Equal:
        case BinaryOperator::FloatPointEqual:
            return "jne";
        case BinaryOperator::NotEqual:
        case BinaryOperator::FloatPointNotEqual:
            return "je";
        default:
            assert(0);
    }
}


const std::string BDD::GetUnsignedSet(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::Greater:
        case BinaryOperator::FloatPointGreater:
            return "seta";
        case BinaryOperator::Lesser:
        case BinaryOperator::FloatPointLesser:
            return "setb";
        case  BinaryOperator::GreaterEqual:
        case  BinaryOperator::FloatPointGreaterEqual:
            return "setnb";
        case BinaryOperator::LesserEqual:
        case BinaryOperator::FloatPointLesserEqual:
            return "setna";
        case BinaryOperator::Equal:
        case BinaryOperator::FloatPointEqual:
            return "sete";
        case BinaryOperator::NotEqual:
        case BinaryOperator::FloatPointNotEqual:
            return "setne";
        default:
            assert(0);
    }
}

const std::string BDD::GetSet(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::Greater:
            return "setg";
        case BinaryOperator::FloatPointGreater:
            return "seta";
        case BinaryOperator::FloatPointLesser:
            return "setb";
        case  BinaryOperator::GreaterEqual:
            return "setge";
        case  BinaryOperator::FloatPointGreaterEqual:
            return "setae";
        case BinaryOperator::FloatPointLesserEqual:
            return "setbe";
        case BinaryOperator::Lesser:
            return "setl";
        case BinaryOperator::LesserEqual:
            return "setle";
        case BinaryOperator::Equal:
            return "sete";
        case BinaryOperator::FloatPointEqual:
            return "setnp";
        case BinaryOperator::NotEqual:
            return "setne";
        case BinaryOperator::FloatPointNotEqual:
            return "setp";
        default:
            assert(0);
    }
}


const std::string BDD::GetRax(int size) {
    if (size == 1){
        return "%al";
    }else if (size == 2){
        return "%ax";
    }else if (size == 4){
        return "%eax";
    }else if (size == 8){
        return "%rax";
    } else{
        assert(0);
    }
}


std::string BDD::GetCastCode(std::string fromTo) {
    auto castCode = castMap.find(fromTo);
    if (castCode  == castMap.end()){
        return "";
    }
    return castCode->second;
}

const std::string BDD::GetRdi(std::shared_ptr<Type> type) {
    if (type ->Size == 1){
        return "%dil";
    }else if (type ->Size == 2){
        return "%di";
    }else if (type ->Size == 4){
        return "%edi";
    }else if (type ->Size == 8){
        return "%rdi";
    } else{
        if (type->IsArrayType() || type ->IsRecordType())
            return "%rdi";
        assert(0);
    }
}

const std::string BDD::GetRcx(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "%cl";
    }else if (type -> Size == 2){
        return "%cx";
    }else if (type -> Size == 4){
        return "%ecx";
    }else if (type -> Size == 8){
        return "%rcx";
    } else{
        if (type->IsArrayType() || type ->IsRecordType())
            return "%rcx";
        assert(0);
    }
}

const std::string BDD::GetRcx(int size) {
    if (size == 1){
        return "%cl";
    }else if (size == 2){
        return "%cx";
    }else if (size == 4){
        return "%ecx";
    }else if (size == 8){
        return "%rcx";
    } else{
        assert(0);
    }
}

const std::string BDD::GetRax(std::shared_ptr<Type> type) {
    if (type->IsArrayType()) {
        return "%rax";
    }else if (type -> Size == 1){
        return "%al";
    }else if (type -> Size == 2){
        return "%ax";
    }else if (type -> Size == 4){
        return "%eax";
    }else if (type -> Size>= 8){
        return "%rax";
    } else{
        assert(0);
    }
}

const std::string BDD::GetRdx(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "%dl";
    }else if (type -> Size == 2){
        return "%dx";
    }else if (type -> Size == 4){
        return "%edx";
    }else if (type -> Size>= 8){
        return "%rdx";
    } else{
        assert(0);
    }
}

const std::string BDD::GetIDivCode(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "idivb";
    }else if (type -> Size == 2){
        return "idivw";
    }else if (type -> Size == 4){
        return "idivw";
    }else if (type -> Size == 8){
        return "idivq";
    } else{
        assert(0);
    }
}


const std::string BDD::GetDivCode(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "divb";
    }else if (type -> Size == 2){
        return "divw";
    }else if (type -> Size == 4){
        return "divw";
    }else if (type -> Size == 8){
        return "divq";
    } else{
        assert(0);
    }
}

const std::string BDD::GetMoveCode2(std::shared_ptr<Type>  type) {
    if (type->IsFloatPointNum()){
        if (type -> Size == 4){
            return "movss";
        }else if (type -> Size == 8){
            return "movsd";
        }
    }else if (type->IsIntegerNum() || type -> IsUnsignedNum()){
        if (type -> Size == 1){
            return "movb";
        }else if (type -> Size == 2){
            return "movw";
        }else if (type -> Size == 4){
            return "movl";
        }else if (type -> Size == 8){
            return "movq";
        }
    }else if(type->IsPointerType()){
        return "movq";
    }else if(type->IsArrayType()){
        return "mov";
    }else if (type->IsBoolType()){
        return "movb";
    }
    assert(0);
}


const std::string BDD::GetMoveCode(std::shared_ptr<Type>  type) {
    if (type->IsFloatPointNum()){
        if (type -> Size == 4){
            return "movss";
        }else if (type -> Size == 8){
            return "movsd";
        }
    }else if (type->IsIntegerNum()){
        if (type -> Size == 1){
            return "movsb";
        }else if (type -> Size == 2){
            return "movsw";
        }else if (type -> Size == 4){
            return "movsl";
        }else if (type -> Size == 8){
            return "movsq";
        }
    }else if(type->IsPointerType()){
        return "movsq";
    }
    assert(0);
}

const std::string BDD::GetMoveCode(int size) {
    if (size == 1){
        return "movsb";
    }else if (size == 2){
        return "movsw";
    }else if (size == 4){
        return "movsl";
    }else if (size  == 8){
        return "movsq";
    }
    assert(0);
}

const std::string BDD::GetMoveCode2(int size) {
    if (size == 1){
        return "movb";
    }else if (size == 2){
        return "movw";
    }else if (size == 4){
        return "movl";
    }else if (size  == 8){
        return "movq";
    }
    assert(0);
}

const std::string BDD::GetDiv(std::shared_ptr<Type> type) {
    if (type -> IsFloatType()){
        return "divss";
    }else if (type -> IsDoubleType()){
        return "divsd";
    }else if (type ->IsULongType()){
        return "div";
    }else{
        return "idiv";
    }
}

const std::string BDD::GetMinus(std::shared_ptr<Type> type) {
    if (type -> IsFloatType()){
        return "subss";
    }else if (type -> IsDoubleType()){
        return "subsd";
    }
    return "sub";
}

const std::string BDD::GetAdd(std::shared_ptr<Type> type) {
    if (type -> IsFloatType()){
        return "addss";
    }else if (type -> IsDoubleType()){
        return "addsd";
    }
    return "add";
}

const std::string BDD::GetSuffix(int size) {
    if (size == 8){
        return "q";
    }else if (size == 4){
        return "l";
    }else if (size == 2){
        return "w";
    }else if (size == 1){
        return "b";
    }
    assert(0);
}

const std::string BDD::GetMul(std::shared_ptr<Type> type) {
    if (type -> IsFloatType()){
        return "mulss";
    }else if (type -> IsDoubleType()){
        return "mulsd";
    }
//    else if (type ->IsULongType()){
//        return "mul";
//    }
    return "imul";
}

const std::string BDD::RepeatN(std::string a,int  n) {
    std::string rt ;
    for (int i = 0; i < n; i++) {
        rt += a;
    }
    return rt;
}

std::shared_ptr<BDD::AstNode> BDD::CastNodeType(
        std::shared_ptr<Type> srcType,std::shared_ptr<Type> destType,std::shared_ptr<BDD::AstNode> destNode) {
    std::shared_ptr<CastNode> castNode;
    if (destType ->IsAliasType())
        destType = destType->GetBaseType();
    if (srcType ->IsAliasType())
        srcType = srcType->GetBaseType();
    if (srcType == destType)
        return destNode;
    auto fromToType = string_format("%s->%s",srcType->Alias,destType->Alias).c_str();
    auto canConvert = castMap.find(fromToType);
    if (canConvert == castMap.end()){
        if (srcType->IsPointerType() && destType->IsPointerType() && srcType->GetBaseType() != destType->GetBaseType()){
        }else if (srcType->IsPointerType() && destType->IsPointerType() && srcType->GetBaseType() == destType->GetBaseType()){
            return destNode;
        }else if (srcType->IsIntType() && destType->IsUnsignedNum()){
        }else if (srcType->IsUnsignedNum() && destType->IsIntegerNum()){
        }else if (srcType->IsLongType() && destType->IsUIntType()){
        }else if (srcType->IsBInType() && destType->IsBoolType()){
        }else if (srcType->IsBInType() && destType->IsFloatPointNum()){
        }else{assert(0);}
    }
    if (auto cstNode = std::dynamic_pointer_cast<ConstantNode>(destNode)){
        if (auto dType = std::dynamic_pointer_cast<BuildInType>(destType)){
            cstNode -> CastValue(dType);
        }else if(auto pType = std::dynamic_pointer_cast<PointerType>(destType)){
            cstNode -> CastValue(Type::LongType);
            cstNode ->Type = pType;
        }else{
            assert(0);
        }
        return destNode;
    }
    castNode = std::make_shared<CastNode>(destNode->Tk);
    castNode->CstNode = destNode;
    castNode->Type = destType;
    return castNode;
}
