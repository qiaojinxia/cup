//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cassert>
#include "CodeGenerate.h"
#include "AstNode.h"
#include <string>
#include "Common.h"
#include "Scope.h"

using namespace BDD;

void BDD::CodeGenerate::Visitor(BDD::BinaryNode *node) {
    if (node->BinOp == BinaryOperator::Assign){
        GenerateAddress(node ->Lhs.get());
        Push();
        node -> Rhs -> Accept(this);
        Store(node -> Type);
        return;
    }else if (node -> BinOp == BinaryOperator::FloatAdd){
        node -> Rhs -> Accept(this);
        node -> Lhs -> Accept(this);
        printf("\t  addss %s,%s\n",Xmm[nextXmm-1],Xmm[nextXmm-2]);
        nextXmm -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatSub){
        node -> Rhs -> Accept(this);
        node -> Lhs -> Accept(this);
        printf("\t  subss %s,%s\n",Xmm[nextXmm-2],Xmm[nextXmm-1]);
        printf("\t  movss %s,%s\n",Xmm[nextXmm-1],Xmm[nextXmm-2]);
        nextXmm -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatMul){
        node -> Rhs -> Accept(this);
        node -> Lhs -> Accept(this);
        printf("\t  mulss %s,%s\n",Xmm[nextXmm-1],Xmm[nextXmm-2]);
        nextXmm -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatDiv){
        node -> Rhs -> Accept(this);
        node -> Lhs -> Accept(this);
        printf("\t  divss %s,%s\n",Xmm[nextXmm-2],Xmm[nextXmm-1]);
        printf("\t  movss %s,%s\n",Xmm[nextXmm-1],Xmm[nextXmm-2]);
        nextXmm -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleAdd){
        node -> Rhs -> Accept(this);
        node -> Lhs -> Accept(this);
        printf("\t  addsd %s,%s\n",Xmm[nextXmm-1],Xmm[nextXmm-2]);
        nextXmm -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleSub){
        node -> Rhs -> Accept(this);
        node -> Lhs -> Accept(this);
        printf("\t  subsd %s,%s\n",Xmm[nextXmm-2],Xmm[nextXmm-1]);
        printf("\t  movsd %s,%s\n",Xmm[nextXmm-1],Xmm[nextXmm-2]);
        nextXmm -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleMul){
        node -> Rhs -> Accept(this);
        node -> Lhs -> Accept(this);
        printf("\t  mulsd %s,%s\n",Xmm[nextXmm-1],Xmm[nextXmm-2]);
        nextXmm -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleDiv){
        node -> Rhs -> Accept(this);
        node -> Lhs -> Accept(this);
        printf("\t  divsd %s,%s\n",Xmm[nextXmm-2],Xmm[nextXmm-1]);
        printf("\t  movsd %s,%s\n",Xmm[nextXmm-1],Xmm[nextXmm-2]);
        nextXmm -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatAssign){
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
        auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
        printf("\t  movss %s(%%rip),%s\n",constNode -> Name.c_str(), Xmm[nextXmm]);
        printf("\t  movss %s,%d(%%rbp)\n", Xmm[nextXmm],varNode ->VarObj ->Offset);
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleAssign){
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
        auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
        printf("\t  movsd %s(%%rip),%s\n",constNode -> Name.c_str(), Xmm[nextXmm]);
        printf("\t  movsd %s,%d(%%rbp)\n", Xmm[nextXmm],varNode ->VarObj ->Offset);
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
            printf("\t  xor %%rdx,%%rdx\n");
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
        case BinaryOperator::PointerAdd:
        {
            auto pType = std::dynamic_pointer_cast<PointerType>(node -> Lhs ->Type) -> Base;
            printf("\t  imul $%d,%%rdi\n",pType-> Size);
            printf("\t  add %%rdi,%%rax\n");
            break;
        }
        case BinaryOperator::PointerSub:
        {
            auto pType = std::dynamic_pointer_cast<PointerType>(node -> Lhs ->Type) -> Base;
            printf("\t  imul $%d,%%rdi\n",pType -> Size);
            printf("\t  sub %%rdi,%%rax\n");
            break;
        }
        case BinaryOperator::PointerDiff:
        {
            auto pType = std::dynamic_pointer_cast<PointerType>(node -> Lhs ->Type) -> Base;
            printf("\t  sub %%rdi,%%rax\n");
            printf("\t  mov $%d, %%rdi\n",pType->Size);
            printf("\t  cqo\n");
            printf("\t  idiv %%rdi\n");
            break;
        }

        case BinaryOperator::ArrayPointerAdd:
        {
            auto pType = std::dynamic_pointer_cast<ArrayType>(node -> Lhs ->Type) ;
            printf("\t  imul $%d,%%rdi\n",pType -> ElementType-> Size);
            printf("\t  add %%rdi,%%rax\n");
            break;
        }
        case BinaryOperator::ArrayPointerSub:
        {
            auto pType = std::dynamic_pointer_cast<ArrayType>(node -> Lhs ->Type);
            printf("\t  imul $%d,%%rdi\n",pType -> ElementType-> Size);
            printf("\t  sub %%rdi,%%rax\n");
            break;
        }
        default:
            assert(0);
    }
}

void BDD::CodeGenerate::Visitor(BDD::ConstantNode *node) {
    if (node ->Type ->IsFloatType()){
        printf("\t  %s %s(%%rip), %s\n", GetMoveCode(node->Type).data(),node->Name.data(),Xmm[nextXmm++]);
        return;
    }
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
    GenerateAddress(node);
    if (node ->Type ->IsIntegerType()){
        Load(node -> Type);
    }
}

void CodeGenerate::Visitor(ProgramNode *node) {
    for(auto& v : scope -> Scope::GetInstance() -> GetConstantTable()){
        printf("%s:\n",v.first.data());
        if (v.second ->Type ->IsFloatType()){
            if (v .second->Type ->Size == 4 ){
                printf("\t.long  %d\n",v.second->Value);
            }else if (v .second->Type ->Size == 8){
                printf("\t.quad  %d\n",v.second->Value);
            }else{
                assert(0);
            }
        }
    }
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
    std::string EndLabel= string_format(".L.end_%d",n);
    std::string BeginLabel = string_format(".L.begin_%d",n);
    PushBreak(EndLabel);
    PushContinue(BeginLabel);
    printf("\t.L.begin_%d:\n",n);
    node -> Cond ->Accept(this);
    printf("\t  cmp $0,%%rax\n");
    printf("\t  je .L.end_%d\n",n);
    node -> Then ->Accept(this);
    printf("\t  jmp .L.begin_%d\n",n);
    printf("\t.L.end_%d:\n",n);
    PopContinue();
    PopBreak();
}

void CodeGenerate::Visitor(DoWhileStmtNode *node) {
    int n = Sequence ++;
    std::string EndLabel= string_format(".L.end_%d",n);
    std::string CondLabel = string_format(".L.cond_%d",n);
    PushBreak(EndLabel);
    PushContinue(CondLabel);
    printf(".L.begin_%d:\n",n);
    node -> Stmt -> Accept(this);
    printf(".L.cond_%d:\n",n);
    node -> Cond ->Accept(this);
    printf("\t  cmp $0, %%rax\n");
    printf("\t  je .L.end_%d\n",n);
    printf("\t  jmp .L.begin_%d\n",n);
    printf(".L.end_%d:\n",n);
    PopContinue();
    PopBreak();
}

void CodeGenerate::Visitor(ForStmtNode *node) {
    int n  = Sequence++;
    std::string EndLabel = string_format(".L.end_%d",n);
    std::string IncrLabel = string_format(".L.incr_%d",n);
    PushBreak(EndLabel);
    PushContinue(IncrLabel);
    if (node -> Init)
        node -> Init ->Accept(this);
    printf(".L.begin_%d:\n",n);
    if (node -> Cond) {
        node->Cond->Accept(this);
        printf("\t  cmp $0,%%rax\n");
        printf("\t  je .L.end_%d\n", n);
    }

    node -> Stmt ->Accept(this);
    printf(".L.incr_%d:\n",n);
    if (node -> Inc){
        node -> Inc ->Accept(this);
    }
    printf("\t  jmp .L.begin_%d\n",n);
    printf(".L.end_%d:\n",n);
    PopContinue();
    PopBreak();
}

void CodeGenerate::Visitor(FunctionNode *node) {
    CurrentFuncName = node -> FuncName;
    printf(".text\n");
#ifdef __linux__
    printf(".globl %s\n",name.data());
    printf("%s:\n",name.data());
#else
    ///macos
    printf(".globl _%s\n",CurrentFuncName.data());
    printf("_%s:\n",CurrentFuncName.data());
#endif
    int offset = 0;
    for (auto &v: node -> Locals) {
        offset += v ->Type ->Size;
        offset = AlignTo(offset,v -> Type -> Align);
        v -> Offset -= offset;
    }
    offset = AlignTo(offset,16);

    printf("\t  push %%rbp\n");
    printf("\t  mov %%rsp, %%rbp\n");
    if (offset > 0 ){
        printf("\t  sub $%d, %%rsp\n",offset); //set stack top
    }
    auto index = 0;
    for (auto &var: node->Params){
        if (var -> Type ->IsFloatType()){
            printf("\t %s %s, %d(%%rbp)\n", GetMoveCode(var->Type).data(),Xmm[nextXmm++],var -> Offset);
        }else if (var -> Type -> IsIntegerType()){
            if (var -> Type -> Size == 1){
                printf("\t  mov %s, %d(%%rbp)\n",Regx8[index++],var -> Offset );
            }else if(var -> Type -> Size == 2){
                printf("\t  mov %s, %d(%%rbp)\n",Regx16[index++],var -> Offset );
            }else if(var -> Type -> Size == 4){
                printf("\t  mov %s, %d(%%rbp)\n",Regx32[index++],var -> Offset );
            }else if(var -> Type -> Size == 8){
                printf("\t  mov %s, %d(%%rbp)\n",Regx64[index++],var -> Offset );
            }else{
                assert(0);
            }
        }else{
            assert(0);
        }
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


void CodeGenerate::Visitor(FuncCallNode *node) {
    for(auto &arg:node -> Args){
        arg ->Accept(this);
        if (arg ->Type->IsIntegerType()){
            Push();
        }
    }
    for (int i = node-> Args.size() -1; i >= 0; --i) {
        if (node->Args[i] ->Type->IsIntegerType()){
            Pop(Regx64[i]);
            //todo push float to stack
        }else if (node->Args[i] ->Type->IsFloatType()){}

    }
    nextXmm = 0;
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

void CodeGenerate::Visitor(DeclarationStmtNode *node) {}



void CodeGenerate::Visitor(StmtExprNode *node) {
    for (auto &s : node ->Stmts) {
        s ->Accept(this);
    }
}

void CodeGenerate::Visitor(UnaryNode *node) {
    switch (node ->Uop) {
        case UnaryOperator::Plus:
            node -> Lhs ->Accept(this);
            break;
        case UnaryOperator::Minus:
            node -> Lhs ->Accept(this);
            printf("\t  neg %%rax\n");
            break;
        case UnaryOperator::Deref:
            GenerateAddress(node);
            Load(node -> Type);
            break;
        case UnaryOperator::Amp:
            GenerateAddress(node -> Lhs.get());
            break;
    }
}

void CodeGenerate::GenerateAddress(AstNode *node) {
    if (auto varNode = dynamic_cast<ExprVarNode *>(node)){
        if (varNode -> Type ->IsFloatType()){
            printf("\t  %s %d(%%rbp),%s\n",GetMoveCode(varNode ->Type).data(),varNode->VarObj->Offset,Xmm[nextXmm]);
            if (varNode ->Type ->Size == 8){
                printf("\t  cvtss2sd  %s, %s\n",Xmm[nextXmm],Xmm[nextXmm]);
            }
            nextXmm ++;
        }else{
            printf("\t  lea %d(%%rbp),%%rax\n",varNode->VarObj ->Offset);
        }
    }else if (auto unaryNode = dynamic_cast<UnaryNode *>(node)){
        if (unaryNode -> Uop == UnaryOperator::Deref){
            unaryNode -> Lhs ->Accept(this);
        }else {
            printf("unaryNode must be defer!\n");
            assert(0);
        }
    }else if (auto memberAccessNode = dynamic_cast<MemberAccessNode *>(node)){
        auto record = std::dynamic_pointer_cast<RecordType>(memberAccessNode ->Lhs -> Type);
        memberAccessNode -> Lhs ->Accept(this);
        auto field = record -> GetField(memberAccessNode -> fieldName);
        printf("\t  sub  $%d,%%rax\n", field ->Offset);
    } else{
        printf("not a value\n");
        assert(0);
    }
}

void CodeGenerate::Visitor(SizeOfExprNode *node) {
    printf("\t  mov $%d,%%rax\n",node ->Type -> Size);
}

void CodeGenerate::Visitor(DeclarationAssignmentStmtNode *node) {
    for (auto &n:node ->AssignNodes) {
        n ->Accept(this);
    }
}

void CodeGenerate::Load(std::shared_ptr<Type> type) {
    if (type -> IsArrayType()){
        return;
    }
    if (type -> Size == 1){
        printf("\t  movsb (%%rax),%%rax\n");
    }else if (type -> Size == 2){
        printf("\t  movsw (%%rax),%%rax\n");
    }else if (type -> Size == 4){
        printf("\t  movsl (%%rax),%%rax\n");
    }else if (type -> Size == 8){
        printf("\t  mov (%%rax),%%rax\n");
    }
}

void CodeGenerate::Store(std::shared_ptr<Type> type) {
    Pop("%rdi");
    if (type -> Size == 1){
        printf("\t  mov %%al,(%%rdi)\n");
    }else if (type -> Size == 2){
        printf("\t  mov %%ax,(%%rdi)\n");
    }else if (type -> Size == 4){
        printf("\t  mov %%eax,(%%rdi)\n");
    }else if (type -> Size == 8){
        printf("\t  mov %%rax,(%%rdi)\n");
    }
}

void CodeGenerate::Visitor(MemberAccessNode *node) {
    auto record = std::dynamic_pointer_cast<RecordType>(node ->Lhs ->Type);
    node -> Lhs-> Accept(this);
    auto field = record -> GetField(node -> fieldName);
    printf("\t  sub  $%d,%%rax\n", field ->Offset);
    Load(node ->Type);
}

void CodeGenerate::Visitor(BreakStmtNode *node) {
    std::string BreakLabel = std::string(currentBreakTarget());
    printf("\t  jmp %s \n",BreakLabel.data());
}

void CodeGenerate::Visitor(ContinueStmtNode *node) {
    std::string ContinueLabel = std::string(currentContinueTarget());
    printf("\t  jmp %s \n",ContinueLabel.data());
}


void CodeGenerate::PushBreak(std::string_view label) {
    BreakStack.push_back(label);
}

void CodeGenerate::PopBreak() {
    BreakStack.pop_back();
}

std::string_view CodeGenerate::currentBreakTarget() {
    return BreakStack.back();
}

void CodeGenerate::PushContinue(std::string_view label) {
    ContinueStack.push_back(label);
}

void CodeGenerate::PopContinue() {
    ContinueStack.pop_back();
}

std::string_view CodeGenerate::currentContinueTarget() {
    return ContinueStack.back();
}

const std::string CodeGenerate::GetMoveCode(std::shared_ptr<Type>  type) {
    if (type -> IsFloatType()){
        if (type -> Size == 4){
            return "movss";
        }else if (type -> Size == 8){
            return "movsd";
        }
    }else if (type->IsIntegerType()){

    }
    assert(0);
}



