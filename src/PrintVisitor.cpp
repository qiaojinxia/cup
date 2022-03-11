//
// Created by a on 2022/3/9.
//

#include <cstdio>
#include <cassert>
#include "PrintVisitor.h"

using namespace BDD;

void PrintVisitor::Visitor(BinaryNode *node) {
    node -> Rhs -> Accept(this);
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
        default:
            assert(0);
            break;

    }
}

void PrintVisitor::Visitor(ConstantNode *node) {
    printf(" %d ",node->Value);
}

void PrintVisitor::Visitor(ProgramNode *node) {
    node -> Lhs ->Accept(this);
    printf("\n");
}
