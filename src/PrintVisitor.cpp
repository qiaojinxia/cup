//
// Created by a on 2022/3/9.
//

#include <cstdio>
#include <cassert>
#include <string>
#include "PrintVisitor.h"


using namespace BDD;

//inorder traversing print formula
void PrintVisitor::Visitor(BinaryNode *node) {
    node -> Lhs ->Accept(this);
    switch (node -> BinOp) {
        case BinaryOperator::Add:
            printf(" + ");
            break;
        case BinaryOperator::Sub:
            printf(" - ");
            break;
        case BinaryOperator::Mul:
            printf(" * ");
            break;
        case BinaryOperator::Div:
            printf(" / ");
            break;
        case BinaryOperator::Mod:
            printf(" %c ",'%');
            break;
        case BinaryOperator::Assign:
            printf(" = ");
            break;
        case BinaryOperator::Greater:
            printf(" > ");
            break;
        case BinaryOperator::GreaterEqual:
            printf(" >= ");
            break;
        case BinaryOperator::Lesser:
            printf(" < ");
            break;
        case BinaryOperator::LesserEqual:
            printf(" <= ");
            break;
        case BinaryOperator::Equal:
            printf(" == ");
            break;
        case BinaryOperator::NotEqual:
            printf(" != ");
            break;
        default:
            assert(0);
    }
    node -> Rhs -> Accept(this);

}

void PrintVisitor::Visitor(ConstantNode *node) {
    printf("%d",node->Value);
}

void PrintVisitor::Visitor(ProgramNode *node) {
    for (auto &s:node -> Funcs ) {
        s ->Accept(this);
    }
}

void PrintVisitor::Visitor(ExprStmtNode *node) {
    if (node-> Lhs){
        node-> Lhs -> Accept(this);
    }
}

void PrintVisitor::Visitor(ExprVarNode *node) {
    printf("%.*s", static_cast<int>(node -> Name.size()), node ->Name.data());
}

void PrintVisitor::Visitor(IfStmtNode *node) {
    printf("if");
    printf("(");
    node -> Cond ->Accept(this);
    printf(") ");
    node -> Then->Accept(this);
    printf(";");
    if (node -> Else){
        printf(" else ");
        node -> Else ->Accept(this);
        printf(";");
    }
}

void PrintVisitor::Visitor(BlockStmtNode *node) {
    printf("{");
    for (auto &s : node -> Stmts ){
        s ->    Accept(this);
    }
    printf("}");
}

void PrintVisitor::Visitor(WhileStmtNode *node) {
    printf("while");
    printf("(");
    node -> Cond -> Accept(this);
    printf(")");
    node -> Then ->Accept(this);
}

void PrintVisitor::Visitor(DoWhileStmtNode *node) {
    printf("do");
    node -> Stmt ->Accept(this);
    printf("while");
    printf("(");
    node -> Cond ->Accept(this);
    printf(")");
}

void PrintVisitor::Visitor(ForStmtNode *node) {
    printf("for");
    printf("(");
    if (node -> Init)
        node -> Init ->Accept(this);
    printf(";");

    if (node -> Cond)
        node -> Cond ->Accept(this);
    printf(";");

    if (node -> Inc)
        node -> Inc ->Accept(this);
    printf(")");

    node -> Stmt ->Accept(this);
}

void PrintVisitor::Visitor(FunctionNode *node) {
    printf("%s", std::string(node->FuncName).c_str());
    printf("(");
    for(int i = 0;i <node->Params.size();i++){
        printf("%s",std::string(node -> Params[i]->Name).c_str());
        if (i != node ->Params.size() -1){
            printf(",");
        }
        printf(")");
        printf("{");
        for (auto &s:node -> Stmts)
            s ->Accept(this);
        printf("}");
    }
}


