//
// Created by a on 2022/3/9.
//

#include "AstNode.h"

using namespace BDD;


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
