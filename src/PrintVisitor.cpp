//
// Created by a on 2022/3/9.
//

#include <cstdio>
#include <cassert>
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
        case BinaryOperator::Assign:
            printf(" = ");
            break;
        default:
            assert(0);
            break;
    }
    node -> Rhs -> Accept(this);

}

void PrintVisitor::Visitor(ConstantNode *node) {
    printf("%d",node->Value);
}

void PrintVisitor::Visitor(ProgramNode *node) {
    for (auto &s:node -> Statements ) {
        s ->Accept(this);
        printf(";\n");
    }
}

void PrintVisitor::Visitor(ExprStmtNode *node) {
    node-> Lhs -> Accept(this);
}

void PrintVisitor::Visitor(ExprVarNode *node) {
    printf("%.*s", static_cast<int>(node -> Name.size()), node ->Name.data());
}


