//
// Created by a on 2022/3/9.
//

#include "AstNode.h"

using namespace BDD;


int BDD::AlignTo(int size, int align) {
    return (size + align - 1) / align * align;
}

void ConstantNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

unsigned long ConstantNode::GetValue() {
    auto s_num = std::string(Token->Content).c_str();
    if (this->Type->Alias == Type::CharType->Alias){
        return (char)atoi(s_num);
    }else if(this->Type->Alias == Type::UCharType->Alias){
        return (unsigned char)atoi(s_num);
    }else if(this->Type->Alias == Type::ShortType->Alias){
        return (short)atoi(s_num);
    }else if(this->Type->Alias == Type::UShortType->Alias){
        return (unsigned short)atoi(s_num);
    }else if(this->Type->Alias == Type::IntType->Alias){
        return atoi(s_num);
    }else if(this->Type->Alias == Type::UIntType->Alias){
        return (unsigned int)atoi(s_num);
    }else if(this->Type->Alias == Type::LongType->Alias){
        return atol(s_num);
    }else if(this->Type->Alias == Type::ULongType->Alias){
        return (unsigned long )atol(s_num);
    }else if(this->Type->Alias == Type::FloatType->Alias){
        float d_num = atof(s_num);
        int *lp_num = (int *) &d_num;
        return *lp_num;
    }else if(this->Type->Alias == Type::DoubleType->Alias){
        double d_num = atof(s_num);
        long *lp_num = (long *) &d_num;
        return *lp_num;
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

void IfStmtNode::Accept(AstVisitor *visitor) {
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
