//
// Created by a on 2022/3/9.
//

#include "AstNode.h"
#include "Common.h"
#include <limits>
using namespace BDD;


int BDD::AlignTo(int size, int align) {
    return (size + align - 1) / align * align;
}

void ConstantNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ConstantNode::CastValue(std::shared_ptr<BuildInType> toType) {
    if (toType == Type::CharType){
        auto * c_v = (char *)&Value;
        *c_v = GetValueT<char>();
    }else if(toType == Type::BoolType){
        auto * c_v = (bool *)&Value;
        *c_v = GetValueT<bool>();
    }else if(toType == Type::UCharType){
        auto * uc_v = (unsigned char *)&Value;
        *uc_v = GetValueT<unsigned char>();
    }else if(toType == Type::ShortType){
        auto * s_v = (short *)&Value;
        *s_v = GetValueT<short>();
    }else if(toType == Type::UShortType){
        auto * us_v = (unsigned short *)&Value;
        *us_v = GetValueT<unsigned short>();
    }else if(toType == Type::IntType){
        auto * i_v = (int *)&Value;
        *i_v = GetValueT<int>();
    }else if(toType == Type::UIntType){
        auto * ui_v = (unsigned int *)&Value;
        *ui_v = GetValueT<unsigned int>();
    }else if(toType == Type::LongType){
        auto * l_v = (long *)&Value;
        *l_v = GetValueT<long>();
    }else if(toType == Type::ULongType){
        auto * ul_v = (unsigned long *)&Value;
        *ul_v = GetValueT<unsigned long>();
    }else if(toType == Type::FloatType){
        auto * f_v = (unsigned int *)&Value;
        auto m = GetValueT<float>();
        auto * nl_p = (unsigned int *)&m;
        *f_v = *nl_p;
    }else if(toType == Type::DoubleType){
        auto * d_v = (unsigned long *)&Value;
        auto m = GetValueT<double>();
        auto * nl_p = (unsigned long *)&m;
        *d_v = *nl_p;
    }
    if (toType ->Size == 8){
    }else{
        Value &= ((unsigned long)1<<(toType->Size * 8)) -1;
    }
    isModify = true;
    Type = toType;
}

template<typename T>
T ConstantNode::GetValue(std::shared_ptr<BuildInType> tp, const char *s_num) {
    if (tp == Type::CharType){
        return (char)atoi(s_num);
    }else if(tp == Type::BoolType){
        return (bool)atoi(s_num);
    }else if(tp == Type::UCharType){
        return (unsigned char)atoi(s_num);
    }else if(tp == Type::ShortType){
        return (short)atoi(s_num);
    }else if(tp == Type::UShortType){
        return (unsigned short)atoi(s_num);
    }else if(tp == Type::IntType){
        char *end_ptr = NULL;
        unsigned long _tmp = strtoul(s_num, &end_ptr, 10);
        return (int)_tmp;
    }else if(tp == Type::UIntType){
        char *end_ptr = NULL;
        unsigned long _tmp = strtoul(s_num, &end_ptr, 10);
        return (unsigned int)_tmp;
    }else if(tp == Type::LongType){
        char *end_ptr = NULL;
        unsigned long _tmp = strtoul(s_num, &end_ptr, 10);
        return (long)_tmp;
    }else if(tp == Type::ULongType){
        char *end_ptr = NULL;
        unsigned long _tmp = strtoul(s_num, &end_ptr, 10);
        return _tmp;
    }else if(tp == Type::FloatType){
        char *end_ptr = NULL;
        unsigned long _tmp = strtoll(s_num, &end_ptr, 10);
        float * f = reinterpret_cast<float *>(&_tmp);
        return *f;
    }else if(tp == Type::DoubleType){
        char *end_ptr = NULL;
        unsigned long _tmp = strtoll(s_num, &end_ptr, 10);
        double * d = reinterpret_cast<double *>(&_tmp);
        return *d;
    }
    assert(0);
}
std::string ConstantNode::GetValueStr(std::shared_ptr<BuildInType> tp, const char *s_num) {
    if (tp == Type::CharType || tp == Type::BoolType){
        return string_format("%hhd",  GetValue<char>(tp,s_num));
    }else if(tp == Type::UCharType){
        return string_format("%hhu",GetValue<unsigned char>(tp,s_num));
    }else if(tp == Type::ShortType){
        return string_format("%hd",GetValue<short>(tp,s_num));
    }else if(tp == Type::UShortType){
        return string_format("%hu",GetValue<unsigned short >(tp,s_num));
    }else if(tp == Type::IntType){
        return string_format("%d",GetValue<int>(tp,s_num));
    }else if(tp == Type::UIntType){
        return string_format("%u",GetValue<unsigned int>(tp,s_num));
    }else if(tp == Type::LongType){
        return string_format("%ld",GetValue<long>(tp,s_num));
    }else if(tp == Type::ULongType){
        return  string_format("%lu",GetValue<unsigned long>(tp,s_num));
    }else if(tp == Type::FloatType){
        return s_num;
    }else if(tp == Type::DoubleType){
        return s_num;
    }
    assert(0);
}

template<typename T>
T ConstantNode::GetValueT() {
    if (!isModify && Tk == nullptr){
        return Value;
    }
    auto s_num = string_format("%lu",Value).c_str();
    if (!isModify){
        s_num = std::string(Tk->Content).c_str();
        if (Tk && is_contains_str(s_num,"0x")){
            long x = hexToDec(s_num,strlen(s_num));
            s_num = string_format("%lu",x).c_str();
        }
        if (Tk &&is_contains_str(std::string(Tk->Content),"0b")){
            long x = binToDec(s_num,strlen(s_num));
            s_num = string_format("%lu",x).c_str();
        }
        if (Tk &&is_contains_str(std::string(Tk->Content),"e")){
            long num_d = atof(s_num);
            s_num = string_format("%lu",num_d).c_str();
        }
        if (this->isChar){
            return this->Value;
        }
        if (Type ->IsFloatType()){
            double num_d = atof(s_num);
            if (num_d > std::numeric_limits<float>::max()){
                auto * ul = reinterpret_cast<unsigned long *>(&num_d);
                s_num = string_format("%lu",*ul).c_str();
            }else{
                float num_f = num_d;
                auto * ui = (unsigned int *)&num_f;
                s_num = string_format("%u",*ui).c_str();
            }
        }
    }
    auto type = std::dynamic_pointer_cast<BuildInType>(this->Type->GetBaseType());
    if (Type == Type::CharType){
        return GetValue<char>(type, s_num);
    }else if(Type == Type::BoolType){
        return GetValue<bool>(type, s_num);
    }else if(Type == Type::UCharType){
        return GetValue<unsigned char>(type, s_num);
    }else if(Type == Type::ShortType){
        return GetValue<short>(type, s_num);
    }else if(Type == Type::UShortType){
        return GetValue<unsigned short>(type, s_num);
    }else if(Type == Type::IntType){
        return GetValue<int>(type, s_num);
    }else if(Type == Type::UIntType){
        return GetValue<unsigned int>(type, s_num);
    }else if(Type == Type::LongType){
        return GetValue<long>(type, s_num);
    }else if(Type == Type::ULongType){
        return GetValue<unsigned long>(type, s_num);
    }else if(Type == Type::FloatType){
        return GetValue<float>(type, s_num);
    }else if(Type == Type::DoubleType){
        return GetValue<double>(type, s_num);
    }
    assert(0);
}

std::string ConstantNode::GetValue() {
    if (!isModify && Tk == nullptr){
        return std::to_string(Value);
    }
    auto s_num = string_format("%lu",Value).c_str();
    if (!isModify){
        s_num = std::string(Tk->Content).c_str();
        if (Tk && is_contains_str(s_num,"0x")){
            long x = hexToDec(s_num,strlen(s_num));
            s_num = string_format("%lu",x).c_str();
        }else if (Tk &&is_contains_str(std::string(Tk->Content),"0b")){
            long x = binToDec(s_num,strlen(s_num));
            s_num = string_format("%lu",x).c_str();
        }
        if (this->isChar){
            return string_format("%d",this->Value);
        }
        if (Type ->IsFloatType()){
            double num_d = atof(s_num);
            if (num_d > std::numeric_limits<float>::max()){
                auto * ul = reinterpret_cast<unsigned long *>(&num_d);
                s_num = string_format("%lu",*ul).c_str();
            }else{
                float num_f = num_d;
                auto * ui = (unsigned int *)&num_f;
                s_num = string_format("%u",*ui).c_str();
            }
        }
    }
    if (this->Type->IsPointerType()){
        return GetValueStr(std::dynamic_pointer_cast<BuildInType>(Type::LongType), s_num);
    }
    return GetValueStr(std::dynamic_pointer_cast<BuildInType>(this->Type->GetBaseType()), s_num);
}

bool ConstantNode::HasSetValue() {
    return Next != nullptr || Sub != 0 || Tk != 0;
}

void BinaryNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ProgramNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ExprStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ExprVarNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void IfElseStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}


void BlockStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void WhileStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void DoWhileStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ForStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void FunctionNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void FuncCallNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ReturnStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void DeclarationStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void StmtExprNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void UnaryNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void SizeOfExprNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void DeclarationAssignmentStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void MemberAccessNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void BreakStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ContinueStmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void CastNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ArrayMemberNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void EmptyNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void AssignNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void AddNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void MinusNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void MulNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void DivNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void ModNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void IncrNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void DecrNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void CmpNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void BitOpNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void TernaryNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void SwitchCaseSmtNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void AndNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

void OrNode::Accept(AstVisitor *visitor) {
    visitor->CurLevel +=1;
    visitor->Visitor(this);
    visitor->CurLevel -=1;
}

