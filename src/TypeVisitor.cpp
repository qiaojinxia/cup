//
// Created by qiaojinxia on 2022/3/24.
//

#include "TypeVisitor.h"
#include "Diag.h"

using namespace BDD;

void TypeVisitor::Visitor(ExprStmtNode *node) {
    if (node -> Lhs){
        node -> Lhs ->Accept(this);
        node -> Type = node -> Lhs ->Type;
    }
}

void TypeVisitor::Visitor(BinaryNode *node) {
    node ->Lhs->Accept(this);
    node ->Rhs ->Accept(this);
    node -> Type =  node -> Rhs -> Type;
    switch (node ->BinOp) {
        case BinaryOperator::Add:
            if (node -> Lhs -> Type ->IsPointerType() && node -> Rhs -> Type ->IsIntegerType()){
                node -> BinOp = BinaryOperator::PointerAdd;
                node ->Type = Type::Pointer;
            }else if (node -> Lhs -> Type -> IsIntegerType() && node -> Rhs -> Type -> IsPointerType()){
                node -> BinOp = BinaryOperator::PointerAdd;
                node ->Type = Type::Pointer;
            }else if (node -> Type -> IsIntegerType() && node -> Rhs -> Type -> IsIntegerType()){
            }else{
                printf("invalid add operation");
                assert(0);
            }
            break;
        case BinaryOperator::Sub:
            if (node -> Lhs -> Type ->IsPointerType() && node -> Rhs -> Type ->IsIntegerType()){
                node -> BinOp = BinaryOperator::PointerSub;
                node ->Type = Type::Pointer;
            }else if (node -> Lhs -> Type -> IsIntegerType() && node -> Rhs -> Type -> IsIntegerType()){
            }else if(node -> Lhs -> Type -> IsPointerType() && node -> Rhs -> Type -> IsPointerType()){
                node -> BinOp = BinaryOperator::PointerDiff;
                node ->Type = Type::Pointer;
            }else{
                printf("invalid sub operation");
                assert(0);
            }
            break;
        case BinaryOperator::PointerAdd:
            node ->Type = Type::Pointer;
        case BinaryOperator::PointerSub:
            node ->Type = Type::Pointer;
        default:
            break;
    }
}

void TypeVisitor::Visitor(ConstantNode *node) {
    node ->Type =  Type::IntType;
}

void TypeVisitor::Visitor(ExprVarNode *node) {
    node -> Type = node ->VarObj ->Type;
}

void TypeVisitor::Visitor(ProgramNode *node) {
    for(auto &fn:node ->Funcs){
        fn ->Accept(this);
    }
}

void TypeVisitor::Visitor(IfStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Then ->Accept(this);
    node ->Else->Accept(this);
}

void TypeVisitor::Visitor(BlockStmtNode *node) {
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
}

void TypeVisitor::Visitor(WhileStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Then ->Accept(this);
}

void TypeVisitor::Visitor(DoWhileStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Stmt ->Accept(this);
}

void TypeVisitor::Visitor(ForStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Stmt ->Accept(this);
    node ->Inc ->Accept(this);
    node ->Init ->Accept(this);
}

void TypeVisitor::Visitor(FunctionNode *node) {
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
}

void TypeVisitor::Visitor(FuncCallNode *node) {
    for(auto &arg:node ->Args){
        arg ->Accept(this);
    }
    node ->Type = Type::IntType;
}

void TypeVisitor::Visitor(ReturnStmtNode *node) {
   node ->Lhs ->Accept(this);
}

void TypeVisitor::Visitor(DeclarationStmtNode *node) {
    for(auto &n:node ->AssignNodes){
        n ->Accept(this);
    }
}

void TypeVisitor::Visitor(StmtExprNode *node) {
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
}

void TypeVisitor::Visitor(UnaryNode *node) {
    node ->Lhs ->Accept(this);
    switch (node -> Uop) {
        case UnaryOperator::Plus:
        case UnaryOperator::Minus:
            node -> Type = node -> Lhs -> Type;
            break;
        case UnaryOperator::Deref:
            if (node -> Lhs -> Type->IsPointerType()){
                node -> Type = std::dynamic_pointer_cast<PointerType>(node -> Lhs->Type)->Base;
            }else{
                printf("invalid defer operation");
            }
            break;
        case UnaryOperator::Amp:
            node -> Type = std::make_shared<PointerType>(node -> Lhs ->Type);
            break;
    }
}

void TypeVisitor::Visitor(SizeOfExprNode *node) {
        node -> Lhs ->Accept(this);
        node -> Type = node -> Lhs ->Type;
}