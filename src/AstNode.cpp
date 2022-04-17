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
