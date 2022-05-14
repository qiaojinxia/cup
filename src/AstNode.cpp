//
// Created by a on 2022/3/9.
//

#include "AstNode.h"
#include "Common.h"

using namespace BDD;


int BDD::AlignTo(int size, int align) {
    return (size + align - 1) / align * align;
}

void ConstantNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}


std::string ConstantNode::GetValue() {
    if (isChange || Token == nullptr){
        return std::to_string(Value);
    }
    auto s_num = std::string(Token->Content).c_str();
    if (is_contains_str(std::string(Token->Content),"0x")){
        return s_num;
    }
    if (is_contains_str(std::string(Token->Content),"0b")){
        return s_num;
    }
    if (this->Type->Alias == Type::CharType->Alias){
        return string_format("%d",(char)atoi(s_num));
    }else if(this->Type->Alias == Type::UCharType->Alias){
        return string_format("%u",(unsigned char)atoi(s_num));
    }else if(this->Type->Alias == Type::ShortType->Alias){
        return string_format("%d",(short)atoi(s_num));
    }else if(this->Type->Alias == Type::UShortType->Alias){
        return string_format("%u",(unsigned short)atoi(s_num));
    }else if(this->Type->Alias == Type::IntType->Alias){
        return string_format("%d",(int)atoi(s_num));
    }else if(this->Type->Alias == Type::UIntType->Alias){
        return string_format("%u",(unsigned int)atoi(s_num));
    }else if(this->Type->Alias == Type::LongType->Alias){
        return string_format("%ld",atol(s_num));
    }else if(this->Type->Alias == Type::ULongType->Alias){
        return  string_format("%s",s_num);
    }else if(this->Type->Alias == Type::FloatType->Alias){
        float d_num = atof(s_num);
        int *lp_num = (int *) &d_num;
        return string_format("%lu",(unsigned long ) *lp_num); ;
    }else if(this->Type->Alias == Type::DoubleType->Alias){
        double d_num = atof(s_num);
        long *lp_num = (long *) &d_num;
        return string_format("%lu",(unsigned long ) *lp_num);
    }
    return 0;
}

void BinaryNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ProgramNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ExprStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ExprVarNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void IfElseStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}


void BlockStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void WhileStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void DoWhileStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ForStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void FunctionNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void FuncCallNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ReturnStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void DeclarationStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void StmtExprNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void UnaryNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void SizeOfExprNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void DeclarationAssignmentStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void MemberAccessNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void BreakStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ContinueStmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void CastNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ArefNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void EmptyNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void AssignNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void AddNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void MinusNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void MulNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void DivNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ModNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void IncrNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void DecrNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void CmpNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void BitOpNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void TernaryNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void SwitchCaseSmtNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void AndNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void OrNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

