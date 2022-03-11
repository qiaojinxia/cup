//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cassert>
#include "CodeGenerate.h"
#include "AstNode.h"

using namespace BDD;

void BDD::CodeGenerate::Visitor(BDD::BinaryNode *node) {
    node->Rhs -> Accept(this);
    Push();
    node -> Lhs ->Accept(this);
    Pop("%rdi");
    switch (node -> BinOp) {
        case BinaryOperator::Add:
            printf("\tadd %%rdi,%%rax\n");
            break;
        case BinaryOperator::Sub:
            printf("\tsub %%rdi,%%rax\n");
            break;
        case BinaryOperator::Mul:
            printf("\tmul %%rdi,%%rax\n");
            break;
        case BinaryOperator::Div:
            printf("\tcqo\n");
            printf("\tidiv %%rdi\n");
            break;
        default:
            assert(0);
            break;
    }
}

void BDD::CodeGenerate::Visitor(BDD::ConstantNode *node) {
    printf("\tmov $%d, %%rax\n",node->Value);
}

void BDD::CodeGenerate::Visitor(BDD::ProgramNode *node) {
    printf("\t.text\n");
#ifdef __linux__
    printf("\t.globl prog\n");
    printf("prog:\n");
#else
    ///macos
    printf("\t.globl _prog\n");
    printf("_prog:\n");
#endif
    printf("\tpush %%rbp\n");
    printf("\tmov %%rsp, %%rbp\n");
    printf("\tsub $32, %%rsp\n");
    node->Lhs ->Accept(this);
    assert(StackLevel == 0);
    printf("\tmov %%rbp,%%rsp\n");
    printf("\tpop %%rbp\n");
    printf("\tret \n");
}

void BDD::CodeGenerate::Push() {
    printf("\tpush %%rax\n");
    StackLevel ++;
}

void CodeGenerate::Pop(const char *reg) {
    printf("\tpop %s\n");
    StackLevel --;
}
