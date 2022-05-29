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

const std::string CodeGenerate::GetReg(int size,int n){
    if (size > 8 || n > 5){
        assert(0);
    }
    return Regx64[size/2][n];
}


void BDD::CodeGenerate::Visitor(BDD::BinaryNode *node) {}

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
    if (node ->Type -> IsFunctionType() || node -> Type -> IsStructType() ){
        GenerateAddress(node);
    }else if (node -> Type -> IsPointerType()){
        printf("\t  mov %d(%%rbp),%%rax\n",node -> VarObj -> Offset);
    }else{
        GenerateAddress(node);
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
    if (node->isStatic)
        return;
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
            printConstant(size,isFloat , isString,node -> Tk);
        }
        node -> isStore = true;
        node = node ->Next;
        root ->isStore = true;
    }
}
void printStruct(std::shared_ptr<ConstantNode>& node);
void printCharPointer(std::shared_ptr<ConstantNode>& node);
void printString(std::shared_ptr<ConstantNode>& node);
void printBuildIn(std::shared_ptr<ConstantNode>& node);
void printArray(std::shared_ptr<ConstantNode>& node);
void PrintConstNode(std::shared_ptr<ConstantNode> cstNode){
    if (!cstNode){
        return;
    }
    if (cstNode->Type->IsStructType()){
        printStruct(cstNode);
    } else if (cstNode->Type->IsPtrCharType()){
        printCharPointer(cstNode);
    }else if (cstNode->Type->IsStringType()){
        printString(cstNode);
    }else if (cstNode->Type->IsBInType()){
        printBuildIn(cstNode);
    }else if (cstNode->Type->IsArrayType()){
        printArray(cstNode ->Next);
    }else{
        assert(0);
    }

}

void CodeGenerate::Visitor(ProgramNode *node) {

    for (auto &dataSeg: scope->Scope::GetInstance()->GetStaticTable()) {
        if (!dataSeg.second.empty()){
            printf("%s\n",dataSeg.first.data());
            printf(".align 4\n");
            for (auto &v: dataSeg.second) {
                printf("%s:\n",v.second->Name.data());
                auto cstNode = v.second;
                    if (dataSeg.first == ".bss"){
                        printf("\t .zero  %d\n", cstNode ->Type ->Size);
                    }else{
                        PrintConstNode(cstNode);
                }
            }
        }
    }
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
    int s_offset = 16;

    for (auto &stmt : node ->Stmts) {
        if (!stmt->Type)
            continue;
        if (auto stmtNode = std::dynamic_pointer_cast<ExprStmtNode>(stmt)){
            auto funcCallNode = std::dynamic_pointer_cast<FuncCallNode>(stmtNode->Lhs);
            if (!funcCallNode)
                continue;
            if(funcCallNode->Type->GetBaseType()->IsStructType()){
                offset +=  funcCallNode->Type->GetBaseType()->Size;
                offset = AlignTo(offset, funcCallNode->Type->GetBaseType() -> Align);
                funcCallNode -> ReturnStructOffset = -offset;
            }
        }
    }

    for (auto &v: node -> Locals) {
        if (v->Type ->IsStructType() && v -> VarAttr -> isParam){
            v -> Offset += s_offset;
            v -> VarAttr ->isInit = true;
            s_offset += v->Type->GetBaseType()->Size;
        }
    }
    for (auto &v: node -> Locals) {
        if (v-> VarAttr->isInit){
            continue;
        }
        offset += v ->Type ->Size;
        offset = AlignTo(offset,v -> Type -> Align);
        v -> Offset = -offset;
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
        }else if (var->Type->IsPointerType() || var ->Type ->IsArrayType()){
            printf("\t  mov %s, %d(%%rbp)\n", GetReg(Type::Pointer->Size,index++).data(),var -> Offset );
        }else if (var->Type->GetBaseType()->IsStructType() || var->Type->GetBaseType()->IsUnionType()){
        }else{
            assert(0);
        }
    }

    //release use reg
    Depth= 0;
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
    auto useReg = std::vector<std::string>();
    int count_f = 0;
    int count_i = 0;
    int tmp_rsp =0;
    //if arg is func need load first
    for(int i = node->Args.size() -1;i>=0;i--){
         if(node->Args[i] ->Type ->IsStructType() || node->Args[i] ->Type->IsUnionType()){
            SetCurTargetReg("%rdi");
            GenerateAddress(node->Args[i].get());
            ClearCurTargetReg();
            PushStructOrUnion(node->Args[i]);
            tmp_rsp += node->Args[i] -> Type->Size;
        }
    }

    if( node ->Type->GetBaseType()->IsStructType()){
        useReg.push_back(GetReg(Type::Pointer->Size,count_i++));
    }
    for(int i = node->Args.size() -1;i>=0;i--){
        Depth = 0;
        auto  arg = node->Args[i];
        bool loadToReg = false;
        auto argType = arg ->Type;
        if(arg ->Type ->IsStructType() || arg ->Type->IsUnionType()){
            continue;
        }
        //Allocation register
        if(argType->IsFunctionType()){
            arg ->Accept(this);
            argType =  arg ->Type ->GetBaseType();
        }else if(argType -> IsArrayType()){
            GenerateAddress(arg.get());
        }else{
            arg ->Accept(this);
        }
        if (argType->IsFloatPointNum()){
            useReg.push_back(Xmm[count_f++]);
        }else if( argType -> IsArrayType()){
            //array type pass pointer
            useReg.push_back(GetReg(Type::VoidType->Size,count_i++));
        }else{
            useReg.push_back(GetReg(argType->Size,count_i++));
        }
        if (argType -> IsStructType() || argType -> IsUnionType())
            continue;
        if (node -> Args.size() != 1){
            //push arg
            if (argType ->IsFloatPointNum()){
                Push(argType->GetBaseType(),Xmm[0]);
            }else{
                Push(argType, GetRax(argType).data());
            }
        }else{
            //handle just one arg
            if (argType ->IsFloatPointNum()){
            }else{
                printf("\t  %s  %s,%s\n", GetMoveCode2(argType).data(), GetRax(argType).data(),useReg.back().data());
            }
        }
    }
    

    int seq = 0;
    //if return struct set first addrss to %%rdi
    if( node ->Type->GetBaseType()->IsStructType()){
        int varOffset = GetStructReturn2Offset();
        if (varOffset != 0){
            printf("\t  lea %d(%%rbp),%s\n",varOffset,useReg[seq].data());
        }else{
            printf("\t  lea %d(%%rbp),%s\n",node->ReturnStructOffset,useReg[seq].data());
        }
        seq++;
    }

    for(auto &arg:node -> Args){
        if (node -> Args.size() != 1){
            if (arg->Type->IsStructType() || arg->Type->IsUnionType()) {
                continue;
            }else if(arg->Type->IsPointerType() || arg -> Type->IsArrayType()){
                Pop(Type::Pointer,useReg[seq].data());
            }else{
                Pop(arg ->Type->GetBaseType(),useReg[seq].data());
            }
        }
        seq ++;
    }

    std::string FuncName(node->FuncName);
    if (node ->FuncPointerOffset)
    {
//        node ->FuncPointerOffset->Accept(this);
        GenerateAddress(node ->FuncPointerOffset.get());
        printf("\t  callq *(%%rax)\n");
    }else{
#ifdef __linux__
        printf("\t  call %s\n",FuncName.data());
#else
        printf("\t  call _%s\n",FuncName.data());
#endif
    }
    if (tmp_rsp)
        printf("\t  add $%d,%%rsp\n",tmp_rsp);
    Depth = 0;
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
            GenerateAddress(node -> Lhs.get());
            Load(node -> Lhs);
            break;
        case UnaryOperator::Addr:
            GenerateAddress(node -> Lhs.get());
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

void CodeGenerate::GenerateAddress(AstNode *node) {
    while (auto castNode = dynamic_cast<CastNode *>(node)){
        node = castNode->CstNode.get();
    }
    if (auto varExprNode = dynamic_cast<ExprVarNode *>(node)){
        //if var use to return and return type is struct that's actually a pointer ,point to caller  reserve stack
        if(varExprNode ->Type ->IsFunctionType()){
#ifdef __linux__
            printf("\t  lea %s(%%rip),%s\n", std::string(varExprNode->Tk->Content).data(), GetCurTargetReg().data());
#else
            printf("\t  lea _%s(%%rip),%s\n", std::string(varExprNode->Tk->Content).data(), GetCurTargetReg().data());
#endif
        }else if(varExprNode ->VarObj ->VarAttr->isStatic){
            printf("\t  lea %s(%%rip),%s\n", varExprNode -> VarObj ->GlobalName.data(), GetCurTargetReg().data());
        }else if(varExprNode ->VarObj ->isPointer){
            printf("\t  mov %d(%%rbp),%s\n", varExprNode -> VarObj -> Offset, GetCurTargetReg().data());
        }else{
            printf("\t  lea %d(%%rbp),%s\n", varExprNode -> VarObj -> Offset, GetCurTargetReg().data());
        }
    }else if(auto constNode = dynamic_cast<ConstantNode *>(node)){
        std::string constName =  std::string(constNode->Name);
        printf("\t  lea %s(%%rip),%s\n",constName.data(),GetCurTargetReg().data());
    }else if (auto unaryNode = dynamic_cast<UnaryNode *>(node)){
        if (unaryNode -> Uop == UnaryOperator::Deref){
            unaryNode ->Lhs->Accept(this);
        }else{
            printf("unaryNode must be defer!\n");
            assert(0);
        }
    }else if (auto memberAccessNode = dynamic_cast<MemberAccessNode *>(node)){
        auto record = std::dynamic_pointer_cast<RecordType>(memberAccessNode -> Lhs -> Type ->GetBaseType());
        memberAccessNode -> Lhs ->Accept(this);
        auto field = record -> GetField(memberAccessNode -> fieldName);
        printf("\t  add  $%d,%s\n", field ->Offset,GetCurTargetReg().data());
    }else if (auto arefNode = dynamic_cast<ArefNode *>(node)){
        auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(arefNode ->Lhs);
        arefNode -> Offset ->Accept(this);
        if (arefNode ->Offset ->Type ->Size == Type::IntType ->Size){
            printf("\t  cdqe\n");
        }
        if(varExprNode->VarObj->VarAttr->isStatic ||varExprNode ->Type ->IsPointerType()){
            SetCurTargetReg("%rdi");
            GenerateAddress(varExprNode.get());
            ClearCurTargetReg();
            printf("\t  lea (%%rdi,%%rax,%d),%s\n",node-> Type->GetBaseType()->Size,GetCurTargetReg().data());
        }else{
            printf("\t  lea %d(%%rdi,%%rax,%d),%s\n", varExprNode ->VarObj ->Offset, node-> Type->GetBaseType()->Size, GetCurTargetReg().data());
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


bool  prevHandle(std::shared_ptr<ConstantNode> node){
    if (node ->Name != "" || node ->isRoot){
        PushStoreOffsetTag(node->Name);
        return true;
    }
    return false;
}


void HandleStore(std::shared_ptr<ConstantNode>& node,void ( * p)(std::shared_ptr<ConstantNode>& node),bool PreNext){
    bool isPush = prevHandle(node);
    if (PreNext)
        node = node->Next;
    p(node);
    if (isPush){
        PopStoreOffsetTag();
    }
}

void printBuildIn(std::shared_ptr<ConstantNode>& node){
    printf("\t %s   %s\n", GetStoreCode(node ->Type ->Size).data(),node->GetValue().data());
}

void printCharPointer(std::shared_ptr<ConstantNode>& node){
    auto cstNode = Scope::GetInstance() ->GetConstantTable().find(node->Name);
    if (cstNode == Scope::GetInstance() ->GetConstantTable().end()){
        Scope::GetInstance() ->PutToConstantTable(node);
    }
    printf("\t .quad   %s\n",std::string(node->Name).data());
}

void printString(std::shared_ptr<ConstantNode>& node){
    printf("\t .asciz   %s\n",std::string(node->Tk->Content).data());
}

void printZero(int num){
    if (num)
        printf("\t .zero   %d\n",num);
}


void printArray(std::shared_ptr<ConstantNode>& node){
    auto cursor = node;
    auto aryType = node->Type;//array fisr element type as arrayType
    void (* p) (std::shared_ptr<ConstantNode>& node);
    if (cursor->Type->IsStructType()){
        p =  printStruct;
    } else if (cursor->Type->IsPtrCharType()){
        p = printCharPointer;
    }else if (cursor->Type->IsStringType()){
        p = printString;
    }else if (cursor->Type->IsBInType()){
        p = printBuildIn;
    }else{
        assert(0);
    }
    while(cursor){
        p(cursor);
        cursor = cursor->Next;
    }
}

void printStruct(std::shared_ptr<ConstantNode>& node){
    auto cursor = node ->Next;
    int offset = 0;
    while(cursor){
        int gap = cursor->Offset - offset;
        printZero(gap);
        if (cursor->Type->IsStructType()){
            printStruct(cursor);
        } else if (cursor->Type->IsPtrCharType()){
            printCharPointer(cursor);
        }else if (cursor->Type->IsStringType()){
            printString(cursor);
        }else if (cursor->Type->IsBInType()){
            printBuildIn(cursor);
        }else if (cursor->Type->IsArrayType()){
            printArray(cursor->Sub);
        }
        offset += cursor->Type->Size;
        cursor = cursor ->Next;
    }
    if (node->Type->IsStructType())
        printZero(node->Type->Size - offset);

}

void storeBuildIn(std::shared_ptr<ConstantNode>& node){
    if (node->Type->IsIntegerNum()){
        if (node ->isStore){
            printf("\t  mov %%rax,(%%rdi)\n");
        }else{
            printf("\t  %s $%s,%d(%%rdi)\n", GetMoveCode2(node->Type).data(),node->GetValue().data(),node->Offset);
        }
    }else if(node-> Type-> IsFloatPointNum()){
        if (node ->isStore){
            printf("\t  mov %s(%%rip),%s\n", CurrentOffsetTag().data(),GetRax(node->Type).data());
            printf("\t  mov %s,%d(%%rdi)\n", GetRax(node->Type).data(),node->Offset);
        }else{
            printf("\t  mov $%s,%s\n", node->GetValue().data(),GetRax(node->Type).data());
            printf("\t  mov %s,%d(%%rdi)\n", GetRax(node->Type).data(),node->Offset);
        }
    }
}

void storePointer(std::shared_ptr<ConstantNode>& node){
    if (node ->Type -> IsPtrCharType()){
        // handle cosnt char *
        printf("\t  lea %s(%%rip),%%rax\n", CurrentOffsetTag().data());
        if (node->Offset){
            printf("\t  add $%d,%%rax\n",node->Offset);
        }
        printf("\t  mov %%rax,(%%rdi)\n");
    }else{
        printf("\t  %s $%s,(%%rdi)\n", GetMoveCode2(node->Type).data(),node->GetValue().data());
    }
}

void storeArray (std::shared_ptr<ConstantNode>&);

void storeString(std::shared_ptr<ConstantNode>& node) {
    auto iter =  Str2IntArrayIterator(node->Tk->Content);
    auto offset = node ->Offset;
    while(iter.has_next()){
        auto outPutIntNode = iter.next();
        printf("\t  %s $%lu,%s\n", GetMoveCode2(outPutIntNode.Size).data(), outPutIntNode.Value, GetRax(outPutIntNode.Size).data());
        printf("\t  %s %s,%d(%%rdi)\n", GetMoveCode2(outPutIntNode.Size).data(), GetRax(outPutIntNode.Size).data(), offset);
        offset += outPutIntNode .Size;
    }
}

void storeHandle(std::shared_ptr<ConstantNode>& node) {
    while (node) {
        if (node->Type->IsBInType()){
            HandleStore(node,storeBuildIn, false);
        }else if(node->Type->IsPointerType()){
            HandleStore(node,storePointer, false);
        }else if(node->Type->IsStructType()){
            HandleStore(node, storeHandle, true);
        }else if(node->Type->IsStringType()){
            HandleStore(node,storeString, false);
        }else if(node->Type->IsArrayType()){
            HandleStore(node,storeArray, false);
        }
        if (node)
            node = node->Next;
        else
            break;
    }
}

void mmStoreArray(std::shared_ptr<ConstantNode> node,std::string src){
    int offset = node->Offset;
    std::shared_ptr<ConstantNode> cursor = node ->Sub;
    if(src.empty()){
        while(cursor){
            int size = 0;
            unsigned long fullNum = 0;
            while(size<8){
                if (!cursor || size + cursor->Type->Size > 8 )
                    break;
                fullNum += cursor->Value << (size * 8);
                size += cursor->Type->Size;
                cursor = cursor->Next;
            }
            if (size <=0)
                break;
            printf("\t  mov $%s,%s\n",  convert_to_hex(fullNum).data(), GetRax(size).data());
            printf("\t  mov %s,%d(%%rdi)\n",  GetRax(size).data(),offset);
            fullNum = 0;
            offset += size;
        }
    }else{
        //copy array from data seg
    }
}

void storeArray(std::shared_ptr<ConstantNode>& node){
    if(node->Type->GetBaseType()->IsStructType()){
        storeHandle(node);
    }else{
        mmStoreArray(node,"");
    }
}




void CodeGenerate::Store(std::shared_ptr<AstNode> node) {
    std::shared_ptr<AstNode> cursor = node;
    std::shared_ptr<Type> type;
    while (auto castNode = std::dynamic_pointer_cast<CastNode>(cursor)){
        cursor = castNode ->CstNode;
    }
    if (auto constNode = std::dynamic_pointer_cast<ConstantNode>(cursor)){
        storeHandle(constNode);
        return;
    }else if (auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(cursor)){
        type = varExprNode->Type;
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
    }else if(type->IsFunctionType()){
        printf("\t  mov %%rax,(%%rdi)\n");
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
    GenerateAddress(node->Lhs.get());
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


void CodeGenerate::PushStructOrUnion(std::shared_ptr<AstNode> node){
    int surplus = node ->Type->Size;
    int offset = 0;
    int curSize = 0;
    //set the struct start addr
    auto recordType = std::dynamic_pointer_cast<RecordType>(node->Type->GetBaseType());
    int index = 0;
    printf("\t  sub $%d,%%rsp\n",node ->Type->Size);
    if (recordType ->Size > 16){
        while(surplus){
            if (surplus / 8 > 0 ){
                curSize = 8;
            }else if (surplus / 4 > 0 ){
                curSize = 4;
            }else if (surplus / 2 > 0 ){
                curSize = 2;
            }else if (surplus == 1){
                curSize = 1;
            }else{
                return;
            }
            printf("\t  %s %d(%%rdi),%s\n", GetMoveCode2(curSize).data(), offset  , GetRax(curSize).data());
            printf("\t  %s %s,%d(%%rsp)\n", GetMoveCode2(curSize).data(), GetRax(curSize).data(), offset);
            index ++;
            offset += curSize;
            surplus -= curSize;
        }
    }else{
        assert(0);
    }
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


void CodeGenerate::Push(std::shared_ptr<Type> ty,const char * reg) {
    auto size = ty ->Size;
    if (ty ->IsAliasType()){
        ty = std::dynamic_pointer_cast<AliasType>(ty)->Base;
    }
    if (ty ->IsStructType() || ty ->IsPointerType() || ty ->IsArrayType() || ty->IsPtrCharType() ){
        size = Type::VoidType->Size;
    }
    if(ty ->IsFloatPointNum()){
        printf("\t  sub $%d, %%rsp          #Push %s\n",size,reg);
        printf("\t  %s %s,(%%rsp)\n", GetMoveCode(ty).data(),reg);
    }else{
        printf("\t  sub $%d, %%rsp          #Push %s\n",size,reg);
        printf("\t  mov %s,(%%rsp)\n",GetRax(ty).data());
    }
    StackLevel ++;
}

void CodeGenerate::Push(std::shared_ptr<Type> ty) {
    auto size = ty ->Size;
    if (ty ->IsAliasType()){
        ty = std::dynamic_pointer_cast<AliasType>(ty)->Base;
    }
    if (ty ->IsStructType() || ty ->IsPointerType() || ty ->IsArrayType() || ty->IsPtrCharType() ){
        size = Type::VoidType->Size;
    }else{
        printf("\t  sub $%d, %%rsp          #Push %s\n",size,GetRax(ty).data());
        printf("\t  mov %s,(%%rsp)\n",GetRax(ty).data());
    }
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
    if (ty ->IsFloatPointNum()){
        if (reg){
            printf("\t  movsd (%%rsp),%s \n",reg);
        }else{
            printf("\t  movsd (%%rsp),%s \n",Xmm[Depth]);
        }
    }else{
        printf("\t  mov (%%rsp),%s           #Pop %s\n",std::string(reg).data(),std::string(reg).data());
    }
    StackLevel --;
    if (!ty->IsBInType()){
        printf("\t  add $%d, %%rsp\n",Type::VoidType->Size);
        return;
    }
    printf("\t  add $%d, %%rsp\n",ty->Size);
}


void CodeGenerate::Visitor(ArefNode *node) {
    auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(node ->Lhs);
    node -> Offset ->Accept(this);
    if (node ->Offset ->Type ->Size == Type::IntType ->Size){
        printf("\t  cdqe\n");
    }
    if (varExprNode && !varExprNode->VarObj->VarAttr->isStatic){
            printf("\t  mov %d(%%rbp,%%rax,%d),%s\n", varExprNode ->VarObj ->Offset,
                   node-> Type->GetBaseType()->Size,GetRax(node->Type).data());
    }else{
        SetCurTargetReg("%rdi");
        GenerateAddress(node ->Lhs.get());
        ClearCurTargetReg();
        printf("\t  mov (%%rdi,%%rax,%d),%s\n",
               node-> Type->GetBaseType()->Size,GetRax(node->Type).data());
    }
}

void CodeGenerate::Visitor(EmptyNode *node) {}


void CodeGenerate::Visitor(AssignNode *node) {
    USeXmm();
    auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(node->Lhs);
    if (varExprNode)
        SetStructReturn2Offset(varExprNode->VarObj->Offset);
    auto constantNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    if (!constantNode){
        node -> Rhs -> Accept(this);
    }
    SetCurTargetReg("%rdi");
    SetStructReturn2Offset(0);
    GenerateAddress(node ->Lhs.get());
    ClearCurTargetReg();
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
   auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
   auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
   printf("\t  mov %s,%s\n", GetRax(node -> Lhs ->Type).data(), GetRcx(node -> Lhs ->Type).data());
   printf("\t  add $%s,%s\n",constNode->GetValue().data(), GetRcx(varExprNode ->Type).data());
   printf("\t  mov %s,%d(%%rbp)\n", GetRcx(varExprNode ->Type).data(), varExprNode->VarObj -> Offset);
}

void CodeGenerate::Visitor(DecrNode *node) {
    node -> Lhs -> Accept(this);
    auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(node -> Lhs);
    auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    printf("\t mov %s,%s\n", GetRax(node -> Lhs ->Type).data(), GetRcx(node -> Lhs ->Type).data());
    printf("\t  sub $%s,%s\n",constNode->GetValue().data(), GetRcx(node -> Lhs ->Type).data());
    printf("\t  mov %s,%d(%%rbp)\n", GetRcx(varExprNode ->Type).data(), varExprNode-> VarObj -> Offset);
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
    IsCmpJmpModule = true;
    std::string branchLabel = string_format(".LT.else%d",n);
    PushJmpLabel(branchLabel);
    node ->Cond->Accept(this);
    node ->Then->Accept(this);
    printf("\t  jmp .LT.end_%d\n",n);
    if (node ->Else){
        printf(".LT.else%d:\n",n);
        node ->Else->Accept(this);
    }
    printf(".LT.end_%d:\n",n);
    PopJmpLabel();
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

std::string CodeGenerate::GetCurTargetReg() {
    if (curTargetReg.empty()){
        return "%rax";
    }
    return curTargetReg.back();
}

void CodeGenerate::SetCurTargetReg(std::string reg) {
    curTargetReg.push_back(reg);
}


void CodeGenerate::ClearCurTargetReg(){
    curTargetReg.pop_back();
}

const int CodeGenerate::GetStructReturn2Offset() {
    return Return2OffsetStack ;
}

const void CodeGenerate::SetStructReturn2Offset(int offset) {
    Return2OffsetStack = offset;
}

void BDD::PushStoreOffsetTag(std::string_view label) {
    OffsetTag.push_back(label);
}

void BDD::PopStoreOffsetTag() {
    OffsetTag.pop_back();
}

std::string_view BDD::CurrentOffsetTag() {
    return OffsetTag.back();
}
