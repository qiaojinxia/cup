//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cassert>
#include "CodeGenerate.h"
#include "AstNode.h"

using namespace BDD;


void BDD::CodeGenerate::Visitor(BDD::BinaryNode *node) {
    if (node->BinOp == BinaryOperator::Assign){
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node->Lhs);
        assert(varNode != nullptr);
        printf("\t  lea %d(%%rbp),%%rax\n",varNode->VarObj->Offset);
        Push();
        node -> Rhs -> Accept(this);
        Pop("%rdi");
        printf("\t  mov %%rax,(%%rdi)\n");
        return;
    }
    node -> Rhs ->Accept(this);
    Push();
    Pop("%rdi");
    node -> Lhs -> Accept(this);
    switch (node -> BinOp) {
        case BinaryOperator::Add:
            printf("\t  add %%rdi,%%rax\n");
            break;
        case BinaryOperator::Sub:
            printf("\t  sub %%rdi,%%rax\n");
            break;
        case BinaryOperator::Mul:
            printf("\t  imul %%rdi,%%rax\n");
            break;
        case BinaryOperator::Div:
            printf("\t  idiv %%rdi\n");
            break;
        case BinaryOperator::Mod:
            printf("\t  push %%rax\n");
            printf("\t  movl +4(%%rsp),%%edx\n");
            printf("\t  movl +0(%%rsp),%%eax\n");
            printf("\t  add  $8,%%rsp \n");
            printf("\t  idiv %%rdi\n");
            printf("\t  mov %%edx,%%eax\n");
            return;
        case BinaryOperator::Greater:
            printf("\t  cmp %%rdi,%%rax\n");
            printf("\t  setg %%al\n");
            printf("\t  movzb %%al,%%rax\n");
            break;
        case BinaryOperator::GreaterEqual:
            printf("\t  cmp %%rdi,%%rax\n");
            printf("\t  setge %%al\n");
            printf("\t  movzb %%al,%%rax\n");
            break;
        case BinaryOperator::Lesser:
            printf("\t  cmp %%rdi,%%rax\n");
            printf("\t  setl %%al\n");
            printf("\t  movzb %%al,%%rax\n");
            break;
        case BinaryOperator::LesserEqual:
            printf("\t  cmp %%rdi,%%rax\n");
            printf("\t  setle %%al\n");
            printf("\t  movzb %%al,%%rax\n");
            break;
        case BinaryOperator::Equal:
            printf("\t  cmp %%rdi,%%rax\n");
            printf("\t  sete %%al\n");
            printf("\t  movzb %%al,%%rax\n");
            break;
        case BinaryOperator::NotEqual:
            printf("\t  cmp %%rdi,%%rax\n");
            printf("\t  setne %%al\n");
            printf("\t  movzb %%al,%%rax\n");
            break;
        default:
            assert(0);
    }
}

void BDD::CodeGenerate::Visitor(BDD::ConstantNode *node) {
    printf("\t  mov $%d, %%rax\n",node->Value);
}

void BDD::CodeGenerate::Visitor(BDD::ExprStmtNode *node) {
    node->Lhs ->Accept(this);
}

void BDD::CodeGenerate::Push() {
    printf("\t  push %%rax\n");
    StackLevel ++;
}

void CodeGenerate::Pop(const char *reg) {
    printf("\t  pop %s\n",reg);
    StackLevel --;
}

void CodeGenerate::Visitor(ExprVarNode *node) {
    printf("\t  lea %d(%%rbp),%%rax\n",node->VarObj->Offset); //the stack grows toward the lower address, so the address is offset negatively
    printf("\t  mov (%%rax),%%rax\n"); //load rax mem address value to rax
}

void CodeGenerate::Visitor(ProgramNode *node) {
    printf("\t.text\n");
#ifdef __linux__
    printf("\t  .globl prog\n");
    printf("prog:\n");
#else
    ///macos
    printf("\t  .globl _prog\n");
    printf("\t_prog:\n");
#endif
    int stackSize = 0;
    for (auto &v: node -> LocalVariables) {
        stackSize += 8;
        v ->Offset = stackSize * -1;
    }
    printf("\t  push %%rbp\n");
    printf("\t  mov %%rsp, %%rbp\n");
    if (stackSize > 0 ){
        printf("\t  sub $%d, %%rsp\n",stackSize); //set stack top
    }

    for (auto &s:node->Statements) {
        s ->Accept(this);
        assert(StackLevel == 0);
    }
    printf("\t  mov %%rbp,%%rsp\n");
    printf("\t  pop %%rbp\n");
    printf("\t  ret \n");
}

void CodeGenerate::Visitor(IfStmtNode *node) {
    int n = Sequence ++;
    node -> Cond ->Accept(this);
    printf("\t  cmp $0,%%rax\n");
    if (node -> Else){
        printf("\t  je .L.else_%d\n",n);
    }else{
        printf("\t  je .L.end_%d\n",n);
    }
    node -> Then->Accept(this);
    printf("\t jmp .L.end_%d\n",n);
    if (node -> Else){
        printf("\t.L.else_%d:\n",n);
        node ->Else->Accept(this);
        printf("\t jmp .L.end_%d\n",n);
    }
    printf("\t.L.end_%d:\n",n);
}

void CodeGenerate::Visitor(BlockStmtNode *node) {
    for (auto &s:node->Stmts) {
        s ->Accept(this);
    }
}

