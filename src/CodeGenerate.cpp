//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cassert>
#include "CodeGenerate.h"
#include "AstNode.h"
#include <string>
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
    node -> Lhs -> Accept(this);
    Pop("%rdi");
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
    if (node->Lhs){
        node->Lhs ->Accept(this);
    }
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
    for (auto &s: node -> Funcs)
        s->Accept(this);
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
        printf(".L.else_%d:\n",n);
        node ->Else->Accept(this);
        printf("\t jmp .L.end_%d\n",n);
    }
    printf(".L.end_%d:\n",n);
}

void CodeGenerate::Visitor(BlockStmtNode *node) {
    for (auto &s:node->Stmts) {
        s ->Accept(this);
    }
}

void CodeGenerate::Visitor(WhileStmtNode *node) {
    int n = Sequence++;
    printf("\t.L.begin_%d:\n",n);
    node -> Cond ->Accept(this);
    printf("\t  cmp $0,%%rax\n");
    printf("\t  je .L.end_%d\n",n);
    node -> Then ->Accept(this);
    printf("\t  jmp .L.begin_%d\n",n);
    printf("\t.L.end_%d:\n",n);
}

void CodeGenerate::Visitor(DoWhileStmtNode *node) {
    int n = Sequence ++;
    printf(".L.begin_%d:\n",n);
    node -> Stmt -> Accept(this);
    node -> Cond ->Accept(this);
    printf("\t  cmp $0, %%rax\n");
    printf("\t  je .L.end_%d\n",n);
    printf("\t  jmp .L.begin_%d\n",n);
    printf(".L.end_%d:\n",n);

}

void CodeGenerate::Visitor(ForStmtNode *node) {
    int n  = Sequence++;
    if (node -> Init)
        node -> Init ->Accept(this);
    printf(".L.begin_%d:\n",n);
    if (node -> Cond) {
        node->Cond->Accept(this);
        printf("\t  cmp $0,%%rax\n");
        printf("\t  je .L.end_%d\n", n);
    }
    node -> Stmt ->Accept(this);
    if (node -> Inc){
        node -> Inc ->Accept(this);
    }
    printf("\t  jmp .L.begin_%d\n",n);
    printf(".L.end_%d:\n",n);
}

void CodeGenerate::Visitor(FunctionNode *node) {
    printf(".text\n");
    CurrentFuncName = node -> FuncName;
#ifdef __linux__
    printf(".globl %s\n",name.data());
    printf("%s:\n",name.data());
#else
    ///macos
    printf(".globl _%s\n",CurrentFuncName.data());
    printf("_%s:\n",CurrentFuncName.data());
#endif
    int stackSize = 0;
    for (auto &v: node -> Locals) {
        stackSize += 8;
        v ->Offset = stackSize * -1;
    }
    stackSize = AlignTo(stackSize,16);

    printf("\t  push %%rbp\n");
    printf("\t  mov %%rsp, %%rbp\n");
    if (stackSize > 0 ){
        printf("\t  sub $%d, %%rsp\n",stackSize); //set stack top
    }

    for (int i = 0;i < node -> Params.size(); i++){
        printf("\t  mov %s, %d(%%rbp)\n",Regx64[i],node -> Params[i] -> Offset);
    }
    for (auto &s:node->Stmts) {
        s ->Accept(this);
        assert(StackLevel == 0);
    }
    printf(".LReturn_%s:\n",CurrentFuncName.data());
    printf("\t  mov %%rbp,%%rsp\n");
    printf("\t  pop %%rbp\n");
    printf("\t  ret \n");
}

int CodeGenerate::AlignTo(int size, int align) {
    return (size + align - 1) / align * align;
}

void CodeGenerate::Visitor(FuncCallNode *node) {
    for(auto &arg:node -> Args){
        arg ->Accept(this);
        Push();
    }
    for (int i = node-> Args.size() -1; i >= 0; --i) {
        Pop(Regx64[i]);
    }
    std::string FuncName(node->FuncName);
#ifdef __linux__
    printf("\t  call %s\n",FuncName.data());
#else
    printf("\t  call _%s\n",FuncName.data());
#endif
}

void CodeGenerate::Visitor(ReturnStmtNode *node) {
    node -> Lhs -> Accept(this);
    printf("\t  jmp .LReturn_%s\n",CurrentFuncName.data());
}

