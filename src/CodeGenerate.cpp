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
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  addss %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatSub){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  subss %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatMul){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  mulss %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatDiv){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  divss %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleAdd){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  addsd %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleSub){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  subsd %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleMul){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  mulsd %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleDiv){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  divsd %s,%s\n", Xmm[Depth - 1], Xmm[Depth - 2]);
        Depth -=1;
        return;
    }else if (node -> BinOp == BinaryOperator::FloatAssign){
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
        auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
        printf("\t  movss %s(%%rip),%s\n",constNode -> Name.c_str(), Xmm[Depth]);
        printf("\t  movss %s,%d(%%rbp)\n", Xmm[Depth], varNode ->VarObj ->Offset);
        return;
    }else if (node -> BinOp == BinaryOperator::DoubleAssign){
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
        auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
        printf("\t  movsd %s(%%rip),%s\n",constNode -> Name.c_str(), Xmm[Depth]);
        printf("\t  movsd %s,%d(%%rbp)\n", Xmm[Depth], varNode ->VarObj ->Offset);
        return;
    }else if (node -> BinOp == BinaryOperator::Mod){
        auto exprNode = std::dynamic_pointer_cast<ExprVarNode>(node ->Lhs);
        if (exprNode){
            node -> Rhs -> Accept(this);
            printf("\t  cdq\n");
            printf("\t  %s %d(%%rbp)\n", GetIDivCode(exprNode->VarObj ->Type).data(), exprNode -> VarObj ->Offset);
        }else{
            node -> Lhs -> Accept(this);
            Push(node ->Lhs ->Type);
            node -> Rhs -> Accept(this);
            printf("\t  mov %%rax,%%rdi\n");
            Pop(node ->Lhs ->Type);
            printf("\t  cdq\n");
            printf("\t  idiv %%rdi\n");
        }
        printf("\t  mov %%edx,%%eax\n");
        return;
    }else if (node -> BinOp == BinaryOperator::Incr){
        node -> Lhs -> Accept(this);
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
        auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
        printf("\t  mov %%rax,%%rcx\n");
        printf("\t  add $%d,%%rcx\n",constNode->Value);
        printf("\t  mov %s,%d(%%rbp)\n", GetRcx(constNode ->Type).data(),varNode->VarObj -> Offset);
        return;
    }else if (node -> BinOp == BinaryOperator::Decr){
        node -> Lhs -> Accept(this);
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
        auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
        if (node -> Lhs -> Type ->IsPointerType()){
            constNode->Value *= node -> Lhs ->Type ->Size;
        }
        printf("\t  mov %%rax,%%rcx\n");
        printf("\t  sub $%d,%%rcx\n",constNode->Value);
        printf("\t  mov %s,%d(%%rbp)\n", GetRcx(constNode ->Type).data(),varNode-> VarObj -> Offset);
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
        case BinaryOperator::And:
            printf("\t  and %%rdi,%%rax\n");
            break;
        case BinaryOperator::Or:
            printf("\t  or %%rdi,%%rax\n");
            break;
        case BinaryOperator::Sar:
            printf("\t  mov %%dil,%%cl\n");
            printf("\t  sar %%cl,%%rax\n");
            break;
        case BinaryOperator::Sal:
            printf("\t  mov %%dil,%%cl\n");
            printf("\t  sal %%cl,%%rax\n");
            break;
        case BinaryOperator::Xor:
            printf("\t  xor %%rdi,%%rax\n");
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
        printf("\t  %s %s(%%rip), %s\n", GetMoveCode(node->Type).data(),node->Name.data(),Xmm[Depth++]);
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
    if (node -> Type -> IsPointerType()){
        printf("\t  mov %d(%%rbp),%%rax\n",node -> VarObj -> Offset);
    }else{
        GenerateAddress(node);
        Load(node -> Type);
    }
}

void CodeGenerate::Visitor(ProgramNode *node) {
    for(auto& v : scope -> Scope::GetInstance() -> GetConstantTable()){
        if (v.second ->Type ->IsFloatType()){
            printf("%s:\n",v.first.data());
            if (v.second->Type ->Size == 4 ){
                auto s_num =  std::string(v.second->Token -> Content).c_str();
                float d_num = atof(s_num);
                int * lp_num = (int *)&d_num;
                printf("\t.quad  %s\n", convert_to_hex(*lp_num).data());
            }else if (v .second->Type ->Size == 8){
                auto s_num =  std::string(v.second->Token -> Content).c_str();
                double d_num = atof(s_num);
                long * lp_num = (long *)&d_num;
                printf("\t.quad  %s\n", convert_to_hex(*lp_num).data());
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
            printf("\t %s %s, %d(%%rbp)\n", GetMoveCode(var->Type).data(), Xmm[Depth++], var -> Offset);
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
        if (!arg ->Type ->IsFloatType()){
            Push();
        }
    }
    for (int i = node-> Args.size() -1; i >= 0; --i) {
        if (node->Args[i] ->Type->IsFloatType()){}
        else{
            Pop(Regx64[i]);
            //todo push float to stack
        }

    }
    Depth = 0;
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
            node -> Lhs ->Accept(this);
            Load(node -> Lhs -> Type);
            break;
        case UnaryOperator::Addr:
            GenerateAddress(node -> Lhs.get());
            break;
    }
}

void CodeGenerate::GenerateAddress(AstNode *node) {
    if (auto varNode = dynamic_cast<ExprVarNode *>(node)){
        if (varNode -> Type ->IsFloatType()){
            printf("\t  %s %d(%%rbp),%s\n",GetMoveCode(varNode ->Type).data(),varNode->VarObj->Offset,Xmm[Depth]);
            Depth ++;
        }else {
            printf("\t  lea %d(%%rbp),%%rax\n",varNode -> VarObj -> Offset);
        }
    }else if (auto unaryNode = dynamic_cast<UnaryNode *>(node)){
        if (unaryNode -> Uop == UnaryOperator::Deref){
            unaryNode ->Lhs->Accept(this);
        }else{
            printf("unaryNode must be defer!\n");
            assert(0);
        }
    }else if (auto memberAccessNode = dynamic_cast<MemberAccessNode *>(node)){
        auto record = std::dynamic_pointer_cast<RecordType>(memberAccessNode ->Lhs -> Type);
        memberAccessNode -> Lhs ->Accept(this);
        auto field = record -> GetField(memberAccessNode -> fieldName);
        printf("\t  sub  $%d,%%rax\n", field ->Offset);
    }else if (auto arefNode = dynamic_cast<ArefNode *>(node)){
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(arefNode ->Lhs);
        arefNode -> Offset ->Accept(this);
        if (auto ptrType  = std::dynamic_pointer_cast<PointerType>(varNode ->Type)){
            printf("\t  mov $%d,%%rcx\n",node-> Type->Size);
            printf("\t  imul %%rax,%%rcx\n");
            printf("\t  lea %d(%%rbp),%%rax\n",varNode ->VarObj ->Offset);
            printf("\t  mov (%%rax),%%rax\n");
            printf("\t  add %%rcx,%%rax\n");
            return;
        }else if (auto arrType  = std::dynamic_pointer_cast<ArrayType>(varNode ->Type)){
            printf("\t  mov $%d,%%rcx\n",arrType ->ElementType ->Size);
            printf("\t  imul %%rax,%%rcx\n");
            printf("\t  lea %d(%%rbp),%%rax\n",varNode ->VarObj ->Offset);
            printf("\t  add %%rcx,%%rax\n");
        }
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
        if (type -> Size == 1){
            return "movsb";
        }else if (type -> Size == 2){
            return "movsw";
        }else if (type -> Size == 4){
            return "movsl";
        }else if (type -> Size == 8){
            return "movsq";
        }
    }
    assert(0);
}

void CodeGenerate::Visitor(CastNode *node) {
    node -> Node ->Accept( this);
    if (node -> Cop == CastOperator::Double){
        printf("\t  cvtss2sd %s, %s\n", Xmm[Depth - 1], Xmm[Depth - 1]);
    }else if  (node -> Cop == CastOperator::Float){
        printf("\t  cvtsd2ss %s, %s\n", Xmm[Depth - 1], Xmm[Depth - 1]);
    }else{
        assert(0);
    }

}

const std::string CodeGenerate::GetIDivCode(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "idivb";
    }else if (type -> Size == 2){
        return "idivw";
    }else if (type -> Size == 4){
        return "idivw";
    }else if (type -> Size == 8){
        return "idivq";
    } else{
        assert(0);
    }
}

void CodeGenerate::Push(std::shared_ptr<Type> ty) {
    printf("\t  sub $%d, %%rsp\n",ty->Size);
    printf("\t  mov %s,(%%rsp)\n",GetRax(ty).data());

}

void CodeGenerate::Pop(std::shared_ptr<Type> ty) {
    printf("\t  mov(%%rsp),%s\n",GetRax(ty).data());
    printf("\t  add $%d, %%rsp\n",ty->Size);

}

const std::string CodeGenerate::GetRax(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "%al";
    }else if (type -> Size == 2){
        return "%ax";
    }else if (type -> Size == 4){
        return "%eax";
    }else if (type -> Size == 8){
        return "%rax";
    } else{
        assert(0);
    }
}

const std::string CodeGenerate::GetRcx(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "%cl";
    }else if (type -> Size == 2){
        return "%cx";
    }else if (type -> Size == 4){
        return "%ecx";
    }else if (type -> Size == 8){
        return "%rcx";
    } else{
        assert(0);
    }
}


void CodeGenerate::Visitor(ArefNode *node) {
    auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node ->Lhs);
    if (node -> Lhs ->Type ->IsPointerType()){
        node -> Offset ->Accept(this);
        printf("\t  mov $%d,%%rcx\n",node-> Type->Size);
        printf("\t  imul %%rax,%%rcx\n");
        printf("\t  lea %d(%%rbp),%%rax\n",varNode ->VarObj ->Offset);
        printf("\t  mov (%%rax),%%rax\n");
        printf("\t  add %%rcx,%%rax\n");
        Load(node->Type);
        return;
    }
    node -> Offset ->Accept(this);
    auto aType = std::dynamic_pointer_cast<ArrayType>(varNode->Type);
    printf("\t  mov $%d,%%rcx\n",aType -> ElementType ->Size);
    printf("\t  imul %%rax,%%rcx\n");
    printf("\t  lea %d(%%rbp),%%rax\n",varNode ->VarObj ->Offset);
    printf("\t  add %%rcx,%%rax\n");
    Load(node ->Type);
}



