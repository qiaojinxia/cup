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

void CodeGenerate::USeXmm() {
    XmmPrevDepth = Depth;
}

void CodeGenerate::ReleaseXmm() {
    Depth = XmmPrevDepth;
}

const std::string CodeGenerate::GetAdd(std::shared_ptr<Type> type) {
    if (type -> IsFloatType()){
        return "addss";
    }else if (type -> IsDoubleType()){
        return "addsd";
    }
    return "add";
}

const std::string CodeGenerate::GetMul(std::shared_ptr<Type> type) {
    if (type -> IsFloatType()){
        return "mulss";
    }else if (type -> IsDoubleType()){
        return "mulsd";
    }
//    else if (type ->IsULongType()){
//        return "mul";
//    }
    return "imul";
}

const std::string CodeGenerate::GetDiv(std::shared_ptr<Type> type) {
    if (type -> IsFloatType()){
        return "divss";
    }else if (type -> IsDoubleType()){
        return "divsd";
    }else if (type ->IsULongType()){
        return "div";
    }else{
        return "idiv";
    }
}

const std::string CodeGenerate::GetMinus(std::shared_ptr<Type> type) {
    if (type -> IsFloatType()){
        return "subss";
    }else if (type -> IsDoubleType()){
        return "subsd";
    }
    return "sub";
}

void BDD::CodeGenerate::Visitor(BDD::BinaryNode *node) {
}

//if constantNode is buildIntype load value rax else not load
void BDD::CodeGenerate::Visitor(BDD::ConstantNode *node) {
    if (node -> Type -> IsBInType()){
        if (node->Type->IsFloatPointNum()) {
            printf("\t  %s %s(%%rip), %s        #FloatConstant %s \n", GetMoveCode(node->Type).data(), node->Name.data(),
                   Xmm[Depth++], node->Name.data());
            return;
        }
        printf("\t  mov $%s, %%rax   #Constant %s\n",node->GetValue().c_str(),node->GetValue().c_str());
    }else if (node->Type->IsPtrCharType()){
        printf("\t  lea %s(%%rip),%%rax\n", node->Name.data());
    }
}

void BDD::CodeGenerate::Visitor(BDD::ExprStmtNode *node) {
    if (node->Lhs){
        node->Lhs ->Accept(this);
    }
}

void CodeGenerate::CmpZero(std::shared_ptr<AstNode> node){
    if (auto cmpNode = std::dynamic_pointer_cast<CmpNode>(node)){
        return;
    }else if (auto andNode = std::dynamic_pointer_cast<AndNode>(node)){
        return;
    }else if (auto orNode = std::dynamic_pointer_cast<OrNode>(node)){
        return;
    }else if (node->Type ->IsFloatType()){
        printf("\t  xorps %s, %s\n",Xmm[Depth],Xmm[Depth]);
        printf("\t  ucomiss %s, %s\n",Xmm[Depth],Xmm[Depth-1]);
    }else if (node->Type ->IsDoubleType()){
        printf("\t  xorpd %s, %s\n",Xmm[Depth],Xmm[Depth]);
        printf("\t  ucomisd %s, %s\n",Xmm[Depth],Xmm[Depth-1]);
    }else{
        printf("\t  cmp $0, %s\n", GetRax(node->Type).data());
    }
    printf("\t  je  %s\n",GetJmpLabel().data());
}

void CodeGenerate::Visitor(ExprVarNode *node) {
    if (node -> Type -> IsPointerType()){
        printf("\t  mov %d(%%rbp),%%rax\n",node -> VarObj -> Offset);
    }else if(node -> Type -> IsStructType()){
        GenerateAddress(node,"%rax");
    }else{
        GenerateAddress(node,"%rax");
        Load(node);
    }
}

void printConstant(int size, bool isFloat,bool isString,std::shared_ptr<Token> token)
{
    if (isString){
        auto s_string =  std::string(token->Content).c_str();
        printf("\t.string  %s\n", s_string);
    }else if (isFloat){
        if (size == 4 ) {
                auto s_num = std::string(token->Content).c_str();
                float d_num = atof(s_num);
                int *lp_num = (int *) &d_num;
                printf("\t.long  %s\n", convert_to_hex(*lp_num).data());
        } else if (size == 8) {
                auto s_num = std::string(token->Content).c_str();
                double d_num = atof(s_num);
                long *lp_num = (long *) &d_num;
                printf("\t.quad  %s\n", convert_to_hex(*lp_num).data());
        }
    }else{
        if (size == 1 ) {
            printf("\t.byte  %d\n", (unsigned int)token->Value);
        }else if (size == 2 ) {
            printf("\t.short  %d\n", (unsigned int)token->Value);
        }else if (size == 4 ) {
            printf("\t.long  %d\n", (unsigned int)token->Value);
        } else if (size == 8) {
            printf("\t.quad  %ld\n", token ->Value);
        }
    }
    return;
}

void ParseInit(std::shared_ptr<ConstantNode> node){
    int size = 0 ;
    bool isFloat = false;
    bool isString = false;
    int offset = 0;
    bool skip = true;
    auto root = node;
    if (node -> Type ->IsFloatPointNum()){
        skip = false;
    }else if (!node ->Type  -> IsBInType() ) {
        if (node->Type->Size >= 48 ){
            skip = false;
        }
    }
    if (node->isRoot){
        node = node ->Next;
    }
    int first = true;
    while (node) {
        if(!(node -> Type ->IsPtrCharType() || !skip)){
            node = node ->Next;
            continue;
        }
        if (first){
            printf("%s:\n", root -> Name.data());
            first = false;
        }
        //single value direct loading mov $num,%rax
        isFloat = node->Type->IsFloatPointNum();
        size = node ->Type ->Size;
        isString = node->Type->IsPtrCharType();
        offset += node ->Type ->Size;
        int gap = AlignTo(offset,node->Type->Align) - offset;
        offset += gap;
        if (gap != 0)
            printf("\t.zero  %d\n",gap);
        if (node -> Sub){
            ParseInit(node ->Sub);
        }else{
            printConstant(size,isFloat , isString,node ->Token);
        }
        node -> isStore = true;
        node = node ->Next;
        root ->isStore = true;
    }
}

void CodeGenerate::Visitor(ProgramNode *node) {
    for (auto &v: scope->Scope::GetInstance()->GetConstantTable()) {
        if (v .second ->Next == nullptr && v .second ->Type->IsIntegerNum()){
            continue;
        }
        //if arry or struct | array | string size <= 48 direct mov instance value to init not store in data
        ParseInit(v.second);
    }
    for (auto &s: node->Funcs)
        s->Accept(this);
}

void CodeGenerate::Visitor(IfElseStmtNode *node) {
    IsCmpJmpModule = true;
    int n = Sequence ++;
    if (node ->Else){
        PushJmpLabel(string_format(".L.else_%d",n));
    }else{
        PushJmpLabel(string_format(".L.end_%d",n));
    }
    node -> Cond ->Accept(this);
    IsCmpJmpModule = false;
    node -> Then->Accept(this);
    printf("\t jmp .L.end_%d\n",n);
    if (node -> Else){
        printf(".L.else_%d:\n",n);
        node ->Else->Accept(this);
        printf("\t jmp .L.end_%d\n",n);
    }
    printf(".L.end_%d:\n",n);
    PopJmpLabel();
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
        if (var->Type->IsFloatPointNum()){
            printf("\t %s %s, %d(%%rbp)\n", GetMoveCode(var->Type).data(), Xmm[Depth++], var -> Offset);
        }else if (var->Type->IsIntegerNum()){
                printf("\t  mov %s, %d(%%rbp)\n",Regx64[var -> Type -> Size / 2][index++],var -> Offset );
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
        if (!arg->Type->IsFloatPointNum()){
            Push(arg->Type);
        }
    }
    for (int i = node-> Args.size() -1; i >= 0; --i) {
        if (node->Args[i]->Type->IsFloatPointNum()){
//            printf("\t  cvtss2sd %s,%s\n",Xmm[Depth],Xmm[Depth]);
            if (Depth > 0){
                Depth--;
            }
        }else if (node->Args[i]->Type->IsPointerType() || node->Args[i]->Type->IsStructType() || node->Args[i]->Type->IsArrayType()){
            Pop(node ->Args[i]->Type, Regx64[4][i]);
        }else{
            Pop(node ->Args[i]->Type, Regx64[node ->Args[i]->Type->GetBaseType()->Size / 2][i]);
            //todo push float to stack
        }

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
            if (node ->Lhs ->Type->IsDoubleType()){
                    printf("\t  movq %s, %%rax\n",Xmm[Depth-1]);
                    printf("\t  movabs $-9223372036854775808,%%rcx\n");
                    printf("\t  xor %%rax, rax\n");
                    printf("\t  xor %%rax, %s\n",Xmm[Depth-1]);
            }else if(node ->Lhs ->Type->IsFloatType()){
                printf("\t  movd %s, %%eax\n",Xmm[Depth-1]);
                printf("\t  xor $2147483648, %%eax\n");
                printf("\t  movd %%eax,%s\n",Xmm[Depth-1]);
            }else {
                printf("\t  neg %s\n", GetRax(node -> Lhs -> Type).data());
            }
            break;
        case UnaryOperator::Deref:
            node -> Lhs ->Accept(this);
            Load(node -> Lhs);
            break;
        case UnaryOperator::Addr:
            GenerateAddress(node -> Lhs.get(),"%rax");
            break;
        case UnaryOperator::BitNot:
            node -> Lhs ->Accept(this);
            printf("\t  xor $-1,%s\n", GetRax(node -> Lhs->Type).data());
            break;
        case UnaryOperator::Not:
            node -> Lhs ->Accept(this);
            printf("\t  cmp $0,%s\n", GetRax(node -> Lhs->Type).data());
            printf("\t  sete %%al\n");
            break;
    }
}

void CodeGenerate::GenerateAddress(AstNode *node,std::string targetReg) {
    while (auto castNode = dynamic_cast<CastNode *>(node)){
        node = castNode->CstNode.get();
    }
    if (auto varNode = dynamic_cast<ExprVarNode *>(node)){
        printf("\t  lea %d(%%rbp),%s\n",varNode -> VarObj -> Offset,targetReg.data());
    }else if(auto constNode = dynamic_cast<ConstantNode *>(node)){
        std::string constName =  std::string(constNode->Name);
        printf("\t  lea %s(%%rip),%s\n",constName.data(),targetReg.data());
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
        printf("\t  add  $%d,%s\n", field ->Offset,targetReg.data());
    }else if (auto arefNode = dynamic_cast<ArefNode *>(node)){
        auto varNode = std::dynamic_pointer_cast<ExprVarNode>(arefNode ->Lhs);
        arefNode -> Offset ->Accept(this);
        if (arefNode ->Offset ->Type ->Size == Type::IntType ->Size){
            printf("\t  cdqe\n");
        }
        if (arefNode ->Lhs ->Type ->IsPointerType()){
            Push(Type::LongType);
            arefNode ->Lhs ->Accept(this);
            Pop(Type::LongType, "%rcx");
            printf("\t  lea (%%rax,%%rcx,%d),%s\n",node-> Type->GetBaseType()->Size,targetReg.data());
        }else{
            printf("\t  lea %d(%%rbp,%%rax,%d),%s\n",varNode ->VarObj ->Offset,node-> Type->GetBaseType()->Size,targetReg.data());
        }
    }else{
        printf("not a value\n");
        assert(0);
    }
}

void CodeGenerate::Visitor(SizeOfExprNode *node) {
    printf("\t  mov $%d,%%rax   #sizeof %d\n",node -> Lhs -> Type  -> Size,node -> Lhs -> Type  -> Size);
}

void CodeGenerate::Visitor(DeclarationAssignmentStmtNode *node) {
    for (auto &n:node ->AssignNodes) {
        n ->Accept(this);
    }
}

void CodeGenerate::Load(AstNode *node) {
    auto type = node -> Type;
    if (auto castNode = dynamic_cast<CastNode *>(node)){
        type = castNode ->CstNode ->Type;
    }
    Load(type);
}

void CodeGenerate::Load(std::shared_ptr<AstNode> node) {
    auto type = node -> Type;
    auto cursor = node;
    while (auto castNode = std::dynamic_pointer_cast<CastNode>(cursor)){ //int a = 0; in b = 3;(long) a + long(b)  load from memory a is int
        cursor = castNode ->CstNode;
    }
    Load(cursor ->Type);
}

void CodeGenerate::Load(std::shared_ptr<Type> type){
    if (type->GetBaseType()->IsFloatPointNum()){
        printf("\t  %s (%%rax),%s\n", GetMoveCode(type ->GetBaseType()).data(),Xmm[Depth++]);
        return;
    }else if(type -> IsPointerType()){
        printf("\t  mov (%%rax),%%rax\n");
        return;
    }else{
        printf("\t  mov (%%rax),%s\n", GetRax(type ->GetBaseType()->Size).data());
    }
}


void CodeGenerate::Store(std::shared_ptr<AstNode> node) {
    std::shared_ptr<AstNode> cursor = node;
    std::shared_ptr<Type> type;
    while (auto castNode = std::dynamic_pointer_cast<CastNode>(cursor)){
        cursor = castNode ->CstNode;
    }
    if (auto constNode = std::dynamic_pointer_cast<ConstantNode>(cursor)){
        std::shared_ptr<ConstantNode> rootNode;
        if (constNode -> isRoot){
            rootNode = constNode;
            constNode = constNode ->Next;
        }
        while(constNode){
            if (constNode -> isStore){
                if (constNode-> Type-> IsPtrCharType()){
                    if (rootNode) {
                        printf("\t  lea %s(%%rip),%%rax\n", rootNode->Name.data());
                        printf("\t  add $%d,%%rax\n",constNode->Offset);
                    }else{
                        printf("\t  lea %s(%%rip),%%rax\n", constNode->Name.data());
                    }
                    printf("\t  mov %%rax,(%%rdi)\n");
                }else if (constNode-> Type-> IsFloatPointNum()){
                    printf("\t  mov %s(%%rip),%s\n",constNode->Name.data() ,GetRax(constNode->Type).data());
                    printf("\t  mov %s,%d(%%rdi)\n", GetRax(constNode->Type).data(),constNode->Offset);
                }else{
                    printf("\t  mov $%d,%%rcx\n",cursor->Type->Size);
                    printf("\t  call _mempcy\n");
                }
            }else{
                if (constNode-> Type-> IsPointerType()){
                    printf("\t  movq $%s,(%%rdi)\n",constNode->GetValue().data());
                }else if (constNode->Type->IsFloatPointNum()){
                    printf("\t  mov $%s,%s\n",constNode->GetValue().data(), GetRax(constNode->Type).data());
                    printf("\t  mov %s,%d(%%rdi)\n" ,GetRax(constNode->Type).data(),constNode->Offset);
                }else if(constNode->Type->IsStringType()){
                    auto iter =  Str2IntArrayIterator(constNode->Token->Content);
                    auto offset = 0;
                    while(iter.has_next()){
                        auto outPutIntNode = iter.next();
                        printf("\t  %s $%lu,%s\n", GetMoveCode(outPutIntNode.Size).data(),outPutIntNode.Value, GetRax(outPutIntNode.Size).data());
                        printf("\t  %s %s,%d(%%rdi)\n",  GetMoveCode(outPutIntNode.Size).data(), GetRax(outPutIntNode.Size).data(), offset);
                        offset += outPutIntNode .Size;
                    }
                }else if (!cursor -> Type -> IsStringType() && !cursor -> Type -> IsStructType() && !cursor -> Type -> IsArrayType()){
                    printf("\t  %s $%s,(%%rdi)\n", GetMoveCode2(constNode->Type).data(),constNode->GetValue().data());
                }else{
                    printf("\t  %s  $%s,%d(%%rdi)\n", GetMoveCode2(constNode->Type).data(), constNode->GetValue().c_str(), constNode -> Offset);
                }
            }
            constNode = constNode->Next;
        }
        return;
    }else if (auto exprNode = std::dynamic_pointer_cast<ExprVarNode>(cursor)){
        type = exprNode->Type;
    }else if(auto binaryNode = std::dynamic_pointer_cast<BinaryNode>(cursor)){
        type = binaryNode->Type;
    }else if(auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(cursor)){
        type = unaryNode->Type;
    }else if(auto funcCallNode = std::dynamic_pointer_cast<FuncCallNode>(cursor)){
        type = funcCallNode->Type;
    }else if(auto arefNode = std::dynamic_pointer_cast<ArefNode>(cursor)){
        type = arefNode->Type;
    }else if(auto ternaryNode = std::dynamic_pointer_cast<TernaryNode>(cursor)){
        type = ternaryNode->Type;
    }
    if (type -> IsPointerType()){
        printf("\t  mov %%rax,(%%rdi)\n");
        return;
    }else if(type->IsFloatPointNum()){
        printf("\t  %s %s,(%%rdi)\n", GetMoveCode(type).data(),Xmm[Depth-1]);
        return;
    }else if (!type ->IsBInType()){
        printf("\t  mov $%d,%%rcx\n",type ->Size);
        printf("\t  call _mempcy\n");
        return;
    }else{
        printf("\t  mov %s,(%%rdi)\n",GetRax(type).data());
        return;
    }
    assert(0);
}

void CodeGenerate::Visitor(MemberAccessNode *node) {
    auto record = std::dynamic_pointer_cast<RecordType>(node ->Lhs ->Type ->GetBaseType());
    GenerateAddress(node->Lhs.get(),"%rax");
    auto field = record -> GetField(node -> fieldName);
    printf("\t  add  $%d,%%rax\n", field ->Offset);
    Load(field ->type);
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

const std::string CodeGenerate::GetMoveCode(int size) {
    if (size == 1){
        return "movb";
    }else if (size == 2){
        return "movw";
    }else if (size == 4){
        return "movl";
    }else if (size  == 8){
        return "movq";
    }
    assert(0);
}

const std::string CodeGenerate::GetMoveCode(std::shared_ptr<Type>  type) {
    if (type->IsFloatPointNum()){
        if (type -> Size == 4){
            return "movss";
        }else if (type -> Size == 8){
            return "movsd";
        }
    }else if (type->IsIntegerNum()){
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

const std::string CodeGenerate::GetMoveCode2(std::shared_ptr<Type>  type) {
    if (type->IsFloatPointNum()){
        if (type -> Size == 4){
            return "movss";
        }else if (type -> Size == 8){
            return "movsd";
        }
    }else if (type->IsIntegerNum()){
        if (type -> Size == 1){
            return "movb";
        }else if (type -> Size == 2){
            return "movw";
        }else if (type -> Size == 4){
            return "movl";
        }else if (type -> Size == 8){
            return "movq";
        }
    }
    assert(0);
}

void CodeGenerate::Visitor(CastNode *node) {
    node->CstNode->Accept(this);
    if (node ->Type == node ->CstNode ->Type || node ->Type ->Alias == node -> CstNode ->Type ->Alias ){
        return;
    }
    auto fromTo = string_format("%s->%s", node->CstNode->Type->Alias, node->Type->Alias);
    auto castCode = GetCastCode(fromTo);
    if (castCode == "NULL"){
        return;
    }else if (castCode == ""){
        printf("%s code not exists!\n",fromTo.data());
        return;
    }

    if (is_contains_str(castCode,"%xmm0")){
        auto xmmReg = Xmm[Depth-1];
        if (!node ->CstNode->Type->IsFloatPointNum()){
            xmmReg = Xmm[Depth++];
        }
        string_replace(castCode,"%xmm0",xmmReg);
    }
    printf("\t  %s \n",castCode.data());
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


const std::string CodeGenerate::GetDivCode(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "divb";
    }else if (type -> Size == 2){
        return "divw";
    }else if (type -> Size == 4){
        return "divw";
    }else if (type -> Size == 8){
        return "divq";
    } else{
        assert(0);
    }
}


void CodeGenerate::Push(std::shared_ptr<Type> ty) {
    auto size = ty ->Size;
    if (ty ->IsAliasType()){
        ty = std::dynamic_pointer_cast<AliasType>(ty)->Base;
    }
    if (ty ->IsStructType() || ty ->IsPointerType() || ty ->IsArrayType() || ty->IsPtrCharType() ){
        size = Type::VoidType->Size;
    }
    printf("\t  sub $%d, %%rsp          #Push %s\n",size,GetRax(ty).data());
    printf("\t  mov %s,(%%rsp)\n",GetRax(ty).data());
    StackLevel ++;

}

void CodeGenerate::Pop(std::shared_ptr<Type> ty) {
    printf("\t  mov(%%rsp),%s           #Pop %s\n",GetRax(ty).data(),GetRax(ty).data());
    StackLevel --;
    if (ty->IsStructType() || ty ->IsArrayType() || ty -> IsPointerType()){
        printf("\t  add $%d, %%rsp\n",Type::VoidType->Size);
        return;
    }
    printf("\t  add $%d, %%rsp\n",ty->Size);
}


void CodeGenerate::Pop(std::shared_ptr<Type> ty,const char *reg) {
    printf("\t  mov (%%rsp),%s           #Pop %s\n",std::string(reg).data(),std::string(reg).data());
    StackLevel --;
    if (!ty->IsBInType()){
        printf("\t  add $%d, %%rsp\n",Type::VoidType->Size);
        return;
    }
    printf("\t  add $%d, %%rsp\n",ty->Size);
}

const std::string CodeGenerate::GetRdx(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "%dl";
    }else if (type -> Size == 2){
        return "%dx";
    }else if (type -> Size == 4){
        return "%edx";
    }else if (type -> Size>= 8){
        return "%rdx";
    } else{
        assert(0);
    }
}

const std::string CodeGenerate::GetRax(std::shared_ptr<Type> type) {
    if (type -> Size == 1){
        return "%al";
    }else if (type -> Size == 2){
        return "%ax";
    }else if (type -> Size == 4){
        return "%eax";
    }else if (type -> Size>= 8){
        return "%rax";
    } else{
        assert(0);
    }
}

const std::string CodeGenerate::GetRcx(int size) {
    if (size == 1){
        return "%cl";
    }else if (size == 2){
        return "%cx";
    }else if (size == 4){
        return "%ecx";
    }else if (size == 8){
        return "%rcx";
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
    node -> Offset ->Accept(this);
    if (node ->Offset ->Type ->Size == Type::IntType ->Size){
        printf("\t  cdqe\n");
    }
    if (node ->Lhs ->Type ->IsPointerType()){
        Push(Type::LongType);
        node ->Lhs ->Accept(this);
        Pop(Type::LongType, "%rcx");
        printf("\t  lea (%%rax,%%rcx,%d),%%rax\n",node-> Type->GetBaseType()->Size);
        Load(node);
        return;
    }
    printf("\t  lea %d(%%rbp,%%rax,%d),%%rax\n",varNode ->VarObj ->Offset,node-> Type->GetBaseType()->Size);
    Load(node);
}

const std::string CodeGenerate::GetRax(int size) {
    if (size == 1){
        return "%al";
    }else if (size == 2){
        return "%ax";
    }else if (size == 4){
        return "%eax";
    }else if (size == 8){
        return "%rax";
    } else{
        assert(0);
    }
}

std::string BDD::CodeGenerate::GetCastCode(std::string fromTo) {
    if (CastMap.empty()){
        CastMap["i8->i32"] = "movsx %al, %eax";
        CastMap["u8->i32"] = "movzbl %al, %eax";
        CastMap["bool->i32"] = "movzbl %al, %eax";
        CastMap["u8->u32"] =  "movzbl %al, %eax";

        CastMap["i16->i32"] = "movswl %ax, %eax";
        CastMap["u16->u32"] = "movzwl %ax, %eax";
        CastMap["i32->f32"] = "cvtsi2ssl %eax, %xmm0";
        CastMap["i32->i64"] = "movsx %eax, %rax";

        CastMap["i16->u32"] = "movzwl %ax, %eax";

        CastMap["u32->u64"] = "NULL";


        CastMap["i32->i8"] =  "movsx %al, %rax";
        CastMap["i32->i16"] =  "movsx %ax, %rax";
        CastMap["i32->u32"] =  "NULL";

        CastMap["i32->u64"] =  "NULL";
        CastMap["u64->i32"] =  "NULL";

        CastMap["u16->i32"] =  "movzwl %ax, %eax";

        CastMap["u64->i64"] =  "NULL";
        CastMap["i64->u64"] =  "NULL";

        CastMap["i32->f64"] = "cvtsi2sdl %eax, %xmm0";

        CastMap["u32->f32"] = "mov %eax, %eax; cvtsi2ssq %rax, %xmm0";
        CastMap["u32->i64"] = "movzx %eax, %rax";
        CastMap["u32->f64"] = "movzx %eax, %rax; cvtsi2sdq %rax, %xmm0";

        CastMap["i64->f32"] = "cvtsi2ssq %rax, %xmm0";
        CastMap["i64->f64"] = "cvtsi2sdq %rax, %xmm0";

        CastMap["u64->f32"] = "cvtsi2ssq %rax, %xmm0";

        CastMap["u64->f64"] =
                "test %rax,%rax; js 1f; pxor %xmm0,%xmm0; cvtsi2sd %rax,%xmm0; jmp 2f; "
                "1: mov %rax,%rdi; and $1,%eax; pxor %xmm0,%xmm0; shr %rdi; "
                "or %rax,%rdi; cvtsi2sd %rdi,%xmm0; addsd %xmm0,%xmm0; 2:";

        CastMap["f32->i8"] = "cvttss2sil %xmm0, %eax; movsbl %al, %eax";
        CastMap["f32->u8"] = "cvttss2sil %xmm0, %eax; movzbl %al, %eax";
        CastMap["f32->i16"] = "cvttss2sil %xmm0, %eax; movswl %ax, %eax";
        CastMap["f32->u16"] = "cvttss2sil %xmm0, %eax; movzwl %ax, %eax";
        CastMap["f32->i32"] = "cvttss2sil %xmm0, %eax";
        CastMap["f32->u32"] = "cvttss2siq %xmm0, %rax";
        CastMap["f32->i64"] = "cvttss2siq %xmm0, %rax";//
        CastMap["f32->u64"] = "cvttss2siq %xmm0, %rax";
        CastMap["f32->f64"] = "cvtss2sd %xmm0, %xmm0";

        CastMap["f64->i8"] = "cvttsd2sil %xmm0, %eax; movsbl %al, %eax";
        CastMap["f64->u8"] = "cvttsd2sil %xmm0, %eax; movzbl %al, %eax";
        CastMap["f64->i16"] = "cvttsd2sil %xmm0, %eax; movswl %ax, %eax";
        CastMap["f64->u16"] = "cvttsd2sil %xmm0, %eax; movzwl %ax, %eax";
        CastMap["f64->i32"] = "cvttsd2sil %xmm0, %eax";//
        CastMap["f64->u32"] = "cvttsd2siq %xmm0, %rax";
        CastMap["f64->f32"] = "cvtsd2ss %xmm0, %xmm0"; //
        CastMap["f64->i64"] = "cvttsd2siq %xmm0, %rax"; //
        CastMap["f64->u64"] = "cvttsd2siq %xmm0, %rax";
    }
    return CastMap[fromTo];
}

const std::string CodeGenerate::GetRdi(std::shared_ptr<Type> type) {
    if (type ->Size == 1){
        return "%dil";
    }else if (type ->Size == 2){
        return "%di";
    }else if (type ->Size == 4){
        return "%edi";
    }else if (type ->Size == 8){
        return "%rdi";
    } else{
        assert(0);
    }
}

const std::string CodeGenerate::GetSet(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::Greater:
            return "setg";
        case BinaryOperator::FloatPointGreater:
            return "seta";
        case BinaryOperator::FloatPointLesser:
            return "setb";
        case  BinaryOperator::GreaterEqual:
            return "setge";
        case  BinaryOperator::FloatPointGreaterEqual:
            return "setae";
        case BinaryOperator::FloatPointLesserEqual:
            return "setbe";
        case BinaryOperator::Lesser:
            return "setl";
        case BinaryOperator::LesserEqual:
            return "setle";
        case BinaryOperator::Equal:
        case BinaryOperator::FloatPointEqual:
            return "sete";
        case BinaryOperator::NotEqual:
        case BinaryOperator::FloatPointNotEqual:
            return "setne";
        default:
            assert(0);
    }
}


void CodeGenerate::Visitor(EmptyNode *node) {}


void CodeGenerate::Visitor(AssignNode *node) {
    USeXmm();
    auto constantNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    if (!constantNode){
        node -> Rhs -> Accept(this);
    }
    GenerateAddress(node ->Lhs.get(),"%rdi");
    Store(node -> Rhs);
    ReleaseXmm();
}

void CodeGenerate::Visitor(AddNode *node) {

    if (node ->BinOp == BinaryOperator::FloatPointAdd){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  %s %s,%s\n",
               GetAdd(node->Lhs->Type).data(),
               Xmm[Depth - 1],
               Xmm[Depth - 2]);
        Depth --;
    }else{
        node -> Rhs -> Accept(this);
        Push(node -> Rhs->Type);
        node -> Lhs -> Accept(this);
        Pop(node->Rhs->Type, GetRdi(node->Rhs->Type).data());
        if (node -> BinOp == BinaryOperator::PointerAdd){
            printf("\t  imul $%d,%s\n", node -> Type ->GetBaseType() -> Size , GetRdi(node -> Rhs->Type).data());
            printf("\t  add %%rdi,%%rax\n");

        }else{
            printf("\t  %s %s,%s\n",
                   GetAdd(node->Lhs->Type).data(),
                   GetRdi(node -> Lhs->Type).data(),
                   GetRax(node -> Lhs->Type).data());
        }
    }
}

void CodeGenerate::Visitor(MinusNode *node) {
    if (node -> BinOp == BinaryOperator::FloatPointMinus){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  %s %s,%s\n",
               GetMinus(node -> Lhs->Type).data(),
               Xmm[Depth - 1],
               Xmm[Depth - 2]);
        Depth --;
    }else{
        node -> Rhs -> Accept(this);
        Push(node -> Rhs->Type);
        node -> Lhs -> Accept(this);
        Pop(node->Rhs->Type, GetRdi(node->Rhs->Type).data());
        if (node -> BinOp == BinaryOperator::PointerSub){
            printf("\t  imul $%d,%s\n", node -> Type ->GetBaseType() -> Size , GetRdi(node -> Rhs->Type).data());
            printf("\t  sub %%rdi,%%rax\n");

        }else if(node-> BinOp == BinaryOperator::PointerDiff){
            printf("\t  sub %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            printf("\t  sar $%d, %s\n", (int)log2(node -> Lhs -> Type -> GetBaseType() ->Size), GetRax(node -> Lhs->Type).data());
        }else{
            printf("\t  %s %s,%s\n",
                   GetMinus(node->Lhs->Type).data(),
                   GetRdi(node -> Lhs->Type).data(),
                   GetRax(node -> Lhs->Type).data());
        }
    }
}

void CodeGenerate::Visitor(MulNode *node) {
    if (node -> BinOp == BinaryOperator::FloatPointMul){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  %s %s,%s\n",
               GetMul(node->Lhs->Type).data(),
               Xmm[Depth - 1],
               Xmm[Depth - 2]);
        Depth --;
    }else{
        node -> Rhs -> Accept(this);
        Push(node -> Rhs->Type);
        node -> Lhs -> Accept(this);
        Pop(node->Rhs->Type, GetRdi(node->Rhs->Type).data());
        printf("\t  %s %s,%s\n",
               GetMul(node->Lhs->Type).data(),
               GetRdi(node -> Lhs->Type).data(),
               GetRax(node -> Lhs->Type).data());
    }
}

void CodeGenerate::Visitor(DivNode *node) {
    if (node -> BinOp == BinaryOperator::FloatPointDiv){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  %s %s,%s\n",
               GetDiv(node->Lhs->Type).data(),
               Xmm[Depth - 1],
               Xmm[Depth - 2]);
        Depth --;
    }else if (node ->BinOp == BinaryOperator::Div
        || node ->BinOp == BinaryOperator::Mod){
        node -> Rhs -> Accept(this);
        Push(node -> Rhs->Type);
        node -> Lhs -> Accept(this);
        Pop(node->Rhs->Type, GetRdi(node->Rhs->Type).data());
        if (node ->Lhs ->Type -> Size == 4){
            printf("\t  xor %%edx,%%edx \n"); //use the highest bit of EAX to expand all bits of EDX
        }else if(node ->Lhs ->Type -> Size == 8){
            printf("\t  xor %%rdx,%%rdx\n"); //use the highest bit of RAX to expand all bits of RDX
        }else{
            assert(0);
        }
        printf("\t  %s %s\n",
               GetDiv(node->Lhs->Type).data(),
               GetRdi(node -> Lhs->Type).data());
    }else if (node ->BinOp == BinaryOperator::IDiv
              || node ->BinOp == BinaryOperator::IMod){
        node -> Rhs -> Accept(this);
        Push(node -> Rhs->Type);
        node -> Lhs -> Accept(this);
        Pop(node->Rhs->Type, GetRdi(node->Rhs->Type).data());
        if (node ->Lhs ->Type -> Size == 4){
            printf("\t  cdq\n"); //use the highest bit of EAX to expand all bits of EDX
        }else if(node ->Lhs ->Type -> Size == 8){
            printf("\t  cqo\n"); //use the highest bit of RAX to expand all bits of RDX
        }else{
            assert(0);
        }
        printf("\t  %s %s\n",
               GetDiv(node->Lhs->Type).data(),
               GetRdi(node -> Lhs->Type).data());
    }
}

void CodeGenerate::Visitor(ModNode *node) {
    auto divNode = static_cast<DivNode>(*node);
    divNode.Accept(this);
    printf("\t  mov %s,%s\n",
               GetRdx(node ->Lhs ->Type).data(),
               GetRax(node ->Lhs ->Type).data());
}

void CodeGenerate::Visitor(IncrNode *node) {
   node -> Lhs -> Accept(this);
   auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
   auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
   printf("\t  mov %s,%s\n", GetRax(node -> Lhs ->Type).data(), GetRcx(node -> Lhs ->Type).data());
   printf("\t  add $%s,%s\n",constNode->GetValue().data(), GetRcx(varNode ->Type).data());
   printf("\t  mov %s,%d(%%rbp)\n", GetRcx(varNode ->Type).data(), varNode->VarObj -> Offset);
}

void CodeGenerate::Visitor(DecrNode *node) {
    node -> Lhs -> Accept(this);
    auto varNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
    auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    printf("\t mov %s,%s\n", GetRax(node -> Lhs ->Type).data(), GetRcx(node -> Lhs ->Type).data());
    printf("\t  sub $%s,%s\n",constNode->GetValue().data(), GetRcx(node -> Lhs ->Type).data());
    printf("\t  mov %s,%d(%%rbp)\n", GetRcx(varNode ->Type).data(), varNode-> VarObj -> Offset);
}

void CodeGenerate::Visitor(CmpNode *node) {
    if (node->Lhs->Type->IsFloatPointNum() || node->Rhs->Type->IsFloatPointNum()){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        printf("\t  ucomiss %s, %s\n",Xmm[Depth - 1] ,Xmm[Depth - 2]);
        Depth -=2;
    }else{
        node -> Rhs -> Accept(this);
        Push(node -> Rhs->Type);
        node -> Lhs -> Accept(this);
        Pop(node->Rhs->Type, GetRdi(node->Rhs->Type).data());
        printf("\t  cmp %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
    }
    if(!IsCmpJmpModule){
            printf("\t  %s  %%al\n", GetSet(node -> BinOp).data());
            printf("\t  movzx %%al,%%eax\n");
    }else {
        printf("\t  %s %s\n", GetReverseJmp(node->BinOp).data(), GetJmpLabel().data());
    }
}

void CodeGenerate::Visitor(BitOpNode *node) {
    node -> Rhs -> Accept(this);
    Push(node -> Rhs->Type);
    node -> Lhs -> Accept(this);
    Pop(node->Rhs->Type, GetRdi(node->Rhs->Type).data());
    switch (node ->BinOp) {
        case BinaryOperator::BitAnd:
            printf("\t  and %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::BitOr:
            printf("\t  or %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::BitSar:
            printf("\t  mov %%dil,%%cl\n");
            printf("\t  sar %%cl,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::BitSal:
            printf("\t  mov %%dil,%%cl\n");
            printf("\t  sal %%cl,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::BitXor:
            printf("\t  xor %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
            break;
        default:
            assert(0);
    }
}

void CodeGenerate::Visitor(TernaryNode *node) {
    int n = Sequence ++;
    node ->Cond->Accept(this);
    printf("\t  jz .LT.else%d\n",n);
    node ->Then->Accept(this);
    if (node ->Else){
        printf(".LT.else%d:\n",n);
        node ->Else->Accept(this);
    }
    printf(".LT.end_%d:\n",n);
}

void CodeGenerate::Visitor(SwitchCaseSmtNode *node) {
    int n = Sequence ++;
    int index = 0;
    std::string EndLabel= string_format(".Ls%d_end\n",n);
    PushBreak(EndLabel);
    node->Value ->Accept(this);
    printf("\t  mov %s,%s\n", GetRax(node->Value->Type).data(), GetRcx(node->Value->Type).data());
    for (auto &branch:node->CaseBranch){
        auto constantNode = std::dynamic_pointer_cast<ConstantNode>(branch.first);
        printf("\t  cmp $%s,%s\n", constantNode->GetValue().data(), GetRcx(node->Value->Type).data());
        printf("\t  je .Ls%d_%d\n",n,index++);
    }
    printf("\t  jmp .Ls%d_d\n",n);
    index = 0;
    for (auto &branch:node->CaseBranch){
        printf(".Ls%d_%d:\n",n,index++);
        for (auto &statement:branch.second){
            statement ->Accept(this);
        }
    }
    printf(".Ls%d_d:\n",n);
    for (auto &statement:node ->DefaultBranch){
        statement ->Accept(this);
    }
    printf(".Ls%d_end:\n",n);
    PopBreak();
}

void CodeGenerate::Visitor(AndNode *node) {
    node ->Lhs ->Accept(this);
    CmpZero(node->Lhs);
    node ->Rhs ->Accept(this);
    CmpZero(node ->Rhs);
}

void CodeGenerate::Visitor(OrNode *node) {
    int n = Sequence ++;
    auto label = string_format(".LI%d\n",n);
    auto endLabel = string_format(".LI%d_End\n",n);
    PushJmpLabel(label);
    node -> Lhs -> Accept(this);
    CmpZero(node ->Lhs);
    printf("\t  jmp %s\n",endLabel.data());
    printf(".LI%d:\n",n);
    PopJmpLabel();
    node -> Rhs -> Accept(this);
    CmpZero(node -> Rhs);
    printf(".LI%d_End:\n",n);
}


const void CodeGenerate::PushJmpLabel(std::string labelName) {
    JmpStack.push_back(labelName);
}

const std::string CodeGenerate::PopJmpLabel() {
    auto backLabel = GetJmpLabel();
    JmpStack.pop_back();
    return std::string(backLabel);
}

const std::string CodeGenerate::GetJmpLabel() {
    auto backLabel = JmpStack.back();
    return std::string(backLabel);
}

std::string CodeGenerate::GetReverseJmp(BinaryOperator anOperator) {
    switch (anOperator) {
        case BinaryOperator::Greater:
            return "jle";
        case BinaryOperator::FloatPointGreater:
            return "jbe";
        case BinaryOperator::FloatPointLesser:
            return "jae";
        case BinaryOperator::GreaterEqual:
            return "jl";
        case BinaryOperator::FloatPointGreaterEqual:
            return "jb";
        case BinaryOperator::FloatPointLesserEqual:
            return "ja";
        case BinaryOperator::Lesser:
            return "jge";
        case BinaryOperator::LesserEqual:
            return "jg";
        case BinaryOperator::Equal:
        case BinaryOperator::FloatPointEqual:
            return "jne";
        case BinaryOperator::NotEqual:
        case BinaryOperator::FloatPointNotEqual:
            return "je";
        default:
            assert(0);
    }
}