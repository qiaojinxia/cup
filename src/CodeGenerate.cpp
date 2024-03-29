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

void CodeGenerate::UseXmm() {
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
    }else if (node->Type->IsStringType()){
        printf("\t  lea %s(%%rip),%%rax\n", node->Name.data());
    }else if (node->Type->IsPointerType()){
        printf("\t  mov $%s, %%rax   #Pointer %s\n",node->GetValue().c_str(),node->GetValue().c_str());
    }
}

void BDD::CodeGenerate::Visitor(BDD::ExprStmtNode *node) {
    if (node->Lhs){
        node->Lhs ->Accept(this);
        Depth =0;
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

//if var is build in type get value if is pointer get pointer value if is func record array get begin address
void CodeGenerate::Visitor(ExprVarNode *node) {
    auto nd = std::make_shared<ExprVarNode>(*node);
    if (IsDirectInStack(nd)){
        if (nd->Type->IsFloatPointNum()){
            printf("\t  %s %d(%%rbp),%s\n", GetMoveCode2(nd->Type).data(), GetVarStackOffset(nd),Xmm[Depth++]);
        }else if(node ->Type -> IsFunctionType()  ||  node -> Type -> IsArrayType()){
            printf("\t  lea %d(%%rbp),%%rax\n", GetVarStackOffset(nd));
        }else if(node ->Type -> IsRecordType()){
            printf("\t  lea %d(%%rbp),%s\n", GetVarStackOffset(nd), SelectReg(GetCurTargetReg(),node ->Type).data());
        }else{
            printf("\t  mov%s %d(%%rbp),%s\n", GetSuffix(nd->Type->Size).data(), GetVarStackOffset(nd),  SelectReg(GetCurTargetReg(),node ->Type).data());
        }
    }else{
        if (node->VarObj->VarAttr->isStatic || node->VarObj->VarAttr->isGlobal)  {
            if (node->Type->IsFloatPointNum()){
                printf("\t  %s %s(%%rip),%s\n", GetMoveCode2(node->Type).data(),node->VarObj->GlobalName.data(),Xmm[Depth++]);
            }else if(node ->Type -> IsFunctionType() || node -> Type -> IsRecordType() ||  node -> Type -> IsArrayType()){
                printf("\t  lea %s(%%rip),%%rax\n", node->VarObj->GlobalName.data());
            }else{
                printf("\t  %s %s(%%rip),%s\n", GetMoveCode2(node->Type).data(),node->VarObj->GlobalName.data(), GetRax(node->Type).data());
            }
            return;
        }
        printf("\t  mov %s(%%rbp),%%rax\n",node->VarObj->GlobalName.data());
    }
}


void printZero(int num){
    if (num)
        printf("\t .zero   %d\n",num);
}

int printStruct(std::shared_ptr<ConstantNode>& node);
int printCharPointer(std::shared_ptr<ConstantNode>& node);
int printString(std::shared_ptr<ConstantNode>& node);
int printBuildIn(std::shared_ptr<ConstantNode>& node);
int printArray(std::shared_ptr<ConstantNode>& node);
void printPointer(std::shared_ptr<ConstantNode>& node);
int printExpr(std::shared_ptr<AstNode>& node,int offset);
void PrintConstNode(std::shared_ptr<ConstantNode> cstNode){
    if (!cstNode){
        return;
    }
    if (cstNode ->isExpr){
        printf("\t %s ", GetStoreCode(cstNode->Type->Size).data());
        printExpr(cstNode->Expr,0);
        printf("\n");
    }else if (cstNode->Type->IsRecordType()){
        int size = cstNode->Type->Size;
        int offset = printStruct(cstNode);
        printZero(size - offset);
    } else if (cstNode->Type->IsPtrCharType()){
        printString(cstNode);
    }else if (cstNode->Type->IsStringType()){
        printString(cstNode);
    }else if (cstNode->Type->IsBInType()){
        printBuildIn(cstNode);
    }else if (cstNode->Type->IsArrayType()){
        int offset = printArray(cstNode ->Next);
        printZero(cstNode->Type->Size - offset);
    }else if (cstNode->Type->IsPointerType()){
        printPointer(cstNode ->Next);
    }else{
        assert(0);
    }
}

void printBinary(std::shared_ptr<BinaryNode>& node,int addOffset){
    if (!node)
        return;
    printExpr(node ->Lhs,addOffset);
    if(auto addNode = std::dynamic_pointer_cast<AddNode>(node)){
        printf("+");
    }else if(auto subNode = std::dynamic_pointer_cast<MinusNode>(node)){
        printf("-");
    }
    printExpr(node ->Rhs,addOffset);
    printf("\n");
}

void printExprVar(std::shared_ptr<ExprVarNode> &node,int addOffset) {
    printf("%s%s", FINDROOTSCOPEPREFIX, std::string(node->Name).data());
}

void printConstantVal(std::shared_ptr<ConstantNode>& node,int addOffset){
    printf("%lu",node->Value + addOffset);
}

void printUnaryNode(std::shared_ptr<UnaryNode>& node,int addOffset){
    printExpr(node->Lhs,addOffset);
}

int printMemberAccessNode(std::shared_ptr<MemberAccessNode>& node,int addOffset){
    auto rdType = std::dynamic_pointer_cast<RecordType>(node->Lhs->Type);
    auto fid = rdType->GetField(node->fieldName);
    return printExpr(node->Lhs,fid->Offset);
}

int printArrayMemberNode(std::shared_ptr<ArrayMemberNode>& node,int addOffset){
    return printExpr(node->Lhs,addOffset);
}

int printExpr(std::shared_ptr<AstNode>& node ,int addOffset ){
    if(auto binaryNode = std::dynamic_pointer_cast<BinaryNode>(node)){
        printBinary(binaryNode,addOffset);
    }else if (auto cstNode = std::dynamic_pointer_cast<ConstantNode>(node)){
        printConstantVal(cstNode,addOffset);
    }else if (auto exprVarNode = std::dynamic_pointer_cast<ExprVarNode>(node)){
        printExprVar(exprVarNode, addOffset);
    }else if (auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(node)){
        printUnaryNode(unaryNode,addOffset);
    }else if (auto maNode = std::dynamic_pointer_cast<MemberAccessNode>(node)){
        printMemberAccessNode(maNode,addOffset);
    }else if (auto amNode = std::dynamic_pointer_cast<ArrayMemberNode>(node)){
        printArrayMemberNode(amNode,addOffset);
    }else{
        assert(0);
    }
    return 8;
}

void preProcessCstNode(std::shared_ptr<ConstantNode> node, bool isInStructOrArray){
    if(!node){
        return;
    }
    if (node ->Type->IsPtrCharType() && isInStructOrArray){
        auto strNode = std::make_shared<ConstantNode>(node->Tk);
        strNode ->Type = Type::StringType;
//        Scope::GetInstance()->PutToConstantTable(strNode);
//        node ->Name = strNode->Name;
    }
    if(node ->Type->IsRecordType() || node ->Type->IsArrayType())
        isInStructOrArray = true;
    preProcessCstNode(node->Sub,isInStructOrArray);
    preProcessCstNode(node->Next,isInStructOrArray);
}

void CodeGenerate::Visitor(ProgramNode *node) {
    for (auto &dataSeg: BDD::Scope::GetInstance()->GetStaticTable()) {
        if (dataSeg.first == ".data"){
            for (auto &v: dataSeg.second) {
                preProcessCstNode(v.second, false);
            }
        }
    }
    for (auto &dataSeg: BDD::Scope::GetInstance()->GetStaticTable()) {
        if (!dataSeg.second.empty()){
            printf("%s\n",dataSeg.first.data());
            printf(".align 8\n");
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
    for (auto &v: BDD::Scope::GetInstance()->GetConstantTable()) {
        preProcessCstNode(v.second, false);
    }
    for (auto &v: BDD::Scope::GetInstance()->GetConstantTable()) {
        if (v .second ->Next == nullptr && v .second ->Type->IsIntegerNum()){
            continue;
        }
        if(v.second ->Type->Size >= 40 || v .second ->Type ->IsFloatPointNum()
        || v .second ->Type ->IsPtrCharType() || v .second ->Type ->IsStringType()){
            printf(".data \n");
            printf(".align 8\n");
            printf("%s:\n", v.second -> Name.data());
            //if arry or struct | array | string size <= 48 direct mov instance value to init not store in data
            PrintConstNode(v.second);
            v.second ->isStore = true;
        }

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
    UseXmm();
    node -> Then->Accept(this);
    ReleaseXmm();
    printf("\t jmp .L.end_%d\n",n);
    if (node -> Else){
        printf(".L.else_%d:\n",n);
        UseXmm();
        node ->Else->Accept(this);
        ReleaseXmm();
        printf("\t jmp .L.end_%d\n",n);
    }
    printf(".L.end_%d:\n",n);
    PopJmpLabel();
}

void CodeGenerate::Visitor(BlockStmtNode *node) {
    for (auto &s:node->Stmts) {
        s ->Accept(this);
    }
    if (node ->ReturnNode)
        node ->ReturnNode -> Accept(this);
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
    //Initialize params stack memory
    for (auto &v: node -> Params) {
        if (v->Type ->IsRecordType()){
            v -> VarAttr ->isReference = true;//if func param is struct actual is  reference pointer to caller stack
            offset += Type::VoidType ->Size;
        }else{
            offset += v ->Type ->Size;
        }
        offset = AlignTo(offset,v -> Type -> Align);
        v -> Offset = -offset;
        v -> VarAttr ->isInit = true;
    }
    //Initialize local variables stack memory
    for (auto &v: node -> Locals) {
        if (v-> VarAttr->isInit){
            continue;
        }
        //empty var continue  such as : struct {} x;
        if (v->Type->Size == 0)
            continue;
        offset += v ->Type ->Size;
        offset = AlignTo(offset,v -> Type -> Align);
        v -> Offset = -offset;
    }


    //if funcCall return struct need allocate memory in  caller
    // then  Pass pointer to callee write
    for (auto &funcCallNode : node ->InnerFunCallStmts) {
        offset +=  funcCallNode->Type->GetBaseType()->Size;
        offset = AlignTo(offset, funcCallNode->Type->GetBaseType() -> Align);
                funcCallNode -> ReturnStructOffset = -offset;
    }

    offset = AlignTo(offset,16);//align stack  multiples of 16

    printf("\t  push %%rbp\n");
    printf("\t  mov %%rsp, %%rbp\n");
    if (offset > 0 ){
        printf("\t  sub $%d, %%rsp\n",offset); //set stack top
    }
    auto index = 0;
    for (auto &var: node->Params){
        if (var->Type->IsFloatPointNum()){
            printf("\t  %s %s, %d(%%rbp)\n", GetMoveCode(var->Type).data(), Xmm[Depth++], var -> Offset);
        }else if (var->Type->IsIntegerNum()){
            printf("\t  mov %s, %d(%%rbp)\n",Regx64[var -> Type -> Size / 2][index++],var -> Offset );
        }else if (var->Type->IsPointerType() || var ->Type ->IsArrayType() ||
                var->Type->GetBaseType()->IsRecordType() ){
            printf("\t  mov %s, %d(%%rbp)\n", GetReg(Type::Pointer->Size,index++).data(),var -> Offset );
        }else{
            assert(0);
        }
    }

    Depth = 0;
    for (auto &s:node->Stmts) {
        s ->Accept(this);
        assert(StackLevel == 0);
    }
    //release use reg
    Depth = 0;
    printf(".LReturn_%s:\n",CurrentFuncName.data());
    if (node->Type->GetBaseType()->IsRecordType())
        printf("\t  mov  -8(%%rbp),%%rax\n");
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
        //if arg is struct copy origin struct as value reference
         if(node->Args[i] ->Type ->IsRecordType() || node->Args[i] ->Type->IsUnionType()){
            SetCurTargetReg("%rdi");
            GenerateAddress(node->Args[i].get());
            ClearCurTargetReg();
            PushStructOrUnion(node->Args[i]);
            tmp_rsp += node->Args[i] -> Type->Size;
        }
    }
    if( node ->Type->GetBaseType()->IsRecordType()){
        useReg.push_back(GetReg(Type::Pointer->Size,count_i++));
    }

    for(int i = node->Args.size() -1;i>=0;i--){
        Depth = 0;
        auto  arg = node->Args[i];
        bool loadToReg = false;
        auto argType = arg ->Type;
        if(arg ->Type ->IsRecordType() || arg ->Type->IsUnionType()){
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
            useReg.emplace_back(Xmm[count_f++]);
        }else if( argType -> IsArrayType() || argType -> IsRecordType()){
            //array type pass pointer
            useReg.push_back(GetReg(Type::VoidType->Size,count_i++));
        }else{
            useReg.push_back(GetReg(argType->Size,count_i++));
        }

        if (node -> Args.size() != 1){
            //push arg
            if (argType ->IsFloatPointNum()){
                Push(argType->GetBaseType(),Xmm[0]);
            }else if(argType -> IsRecordType() || argType -> IsUnionType()){
                Push(Type::VoidType, GetRax(Type::VoidType).data());
            }else{
                Push(argType, GetRax(argType).data());
            }
        }else{
            //handle just one arg
            if (argType ->IsFloatPointNum()){
            }else if(argType -> IsRecordType() || argType -> IsUnionType()){
                printf("\t  %s  %s,%s\n", GetMoveCode2(Type::VoidType).data(), GetRax(Type::VoidType).data(),useReg.back().data());
            }else{
                printf("\t  %s  %s,%s\n", GetMoveCode2(argType).data(), GetRax(argType).data(),useReg.back().data());
            }
        }
    }
    

    int seq = 0;
    //if return struct set first addrss to %%rdi
    if( node ->Type->GetBaseType()->IsRecordType()){
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
            if (arg->Type->IsRecordType() || arg->Type->IsUnionType()) {
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
     if (node->Type->IsRecordType() && node->ReturnOffset){
        SetCurTargetReg("%rsi");
        GenerateAddress(node->Lhs.get());
        ClearCurTargetReg();
        printf("\t  mov %d(%%rbp),%%rdi\n",node->ReturnOffset);
        auto oi = OffsetInfo("%%rdi",0, true);
        Store(node->Lhs,&oi);
    }else{
        node -> Lhs -> Accept(this);
    }
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
        case UnaryOperator::Deref://*
            node->Lhs->Accept(this);
            if (!node->Type->IsArrayType() && !node->Type->IsRecordType())
                Load(node->Lhs);
            break;
        case UnaryOperator::Addr://&
            GenerateAddress(node -> Lhs.get());
            break;
        case UnaryOperator::BitNot:
            node -> Lhs ->Accept(this);
            printf("\t  xor $-1,%s\n", GetRax(node -> Lhs->Type).data());
            break;
        case UnaryOperator::Incr:
            if (node->Lhs->Type->IsFloatPointNum()){
                node->Lhs->Accept(this);
                printf("\t  %s %s(%%rip), %s\n", GetMoveCode2(node->Lhs->Type).data(),node->IncrOrDecrConstantTag->Name.data(),Xmm[Depth]);
                printf("\t  %s %s, %s\n", GetAdd(node->Lhs->Type).data(),Xmm[Depth],Xmm[Depth-1]);
                Depth -= 1;
                if (IsDirectInStack(node->Lhs)){
                    printf("\t  %s %s, %d(%%rbp)\n", GetMoveCode2(node->Lhs->Type).data(),Xmm[Depth], GetVarStackOffset(node->Lhs));
                }else{
                    GenerateAddress(node->Lhs.get());
                    printf("\t  %s %s, (%%rax)\n", GetMoveCode2(node->Lhs->Type).data(),Xmm[Depth]);
                }
            }else{
                int size = 1;
                if (node->Lhs->Type->IsPointerType()){
                    size = node->Lhs->Type->GetBaseType()->Size;
                }
                if (IsDirectInStack(node->Lhs)){
                    printf("\t  add%s $%d,%d(%%rbp)\n",GetSuffix(node->Lhs->Type->Size).data(),size,GetVarStackOffset(node->Lhs));
                }else{
                    GenerateAddress(node->Lhs.get());
                    printf("\t  add%s $%d,(%%rax)\n",GetSuffix(node->Lhs->Type->Size).data(),size);
                }
                if (node ->Level > 3)
                    node->Lhs->Accept(this);
            }
            break;
        case UnaryOperator::Decr:
            if (node->Lhs->Type->IsFloatPointNum()){
                node->Lhs->Accept(this);
                printf("\t  %s %s(%%rip), %s\n", GetMoveCode2(node->Lhs->Type).data(),node->IncrOrDecrConstantTag->Name.data(),Xmm[Depth]);
                printf("\t  %s %s, %s\n", GetMinus(node->Lhs->Type).data(),Xmm[Depth],Xmm[Depth-1]);
                Depth -= 1;
                if (IsDirectInStack(node->Lhs)){
                    printf("\t  %s %s, %d(%%rbp)\n", GetMoveCode2(node->Lhs->Type).data(),Xmm[Depth], GetVarStackOffset(node->Lhs));
                }else{
                    GenerateAddress(node->Lhs.get());
                    printf("\t  %s %s, (%%rax)\n", GetMoveCode2(node->Lhs->Type).data(),Xmm[Depth]);
                }
            }else{
                int size = 1;
                if (node->Lhs->Type->IsPointerType()){
                    size = node->Lhs->Type->GetBaseType()->Size;
                }
                if (IsDirectInStack(node->Lhs)){
                    printf("\t  sub%s $%d,%d(%%rbp)\n",GetSuffix(node->Lhs->Type->Size).data(),size,GetVarStackOffset(node->Lhs));
                }else{
                    GenerateAddress(node->Lhs.get());
                    printf("\t  sub%s $%d,(%%rax)\n",GetSuffix(node->Lhs->Type->Size).data(),size);
                }
                if (node ->Level > 3)
                    node->Lhs->Accept(this);
            }
            break;
        case UnaryOperator::Not:
            node -> Lhs ->Accept(this);
            printf("\t  cmp $0,%s\n", GetRax(node -> Lhs->Type).data());
            printf("\t  sete %%al\n");
            break;
    }
}

//if can Direct positioning through rbp register return true for example int long double store in local stack
// if var is static not direct need first generateAddress then dereference
// for exmaple static var store in data segment 、*y .s all can't index by %d(%rbp) need dereference first
bool  CodeGenerate::IsDirectInStack(std::shared_ptr<AstNode> node){
    if(auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(node)){
        if (varExprNode->VarObj->VarAttr->isReference || varExprNode->VarObj->VarAttr->isStatic || varExprNode->VarObj->VarAttr->isGlobal)
            return false;
    }else if(auto memberAccessNode = std::dynamic_pointer_cast<MemberAccessNode>(node)){
        return IsDirectInStack(memberAccessNode->Lhs);
    }else if(auto arrayMemberNode = std::dynamic_pointer_cast<ArrayMemberNode>(node)){
        return IsDirectInStack(arrayMemberNode->Lhs);
    }else if(auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(node)){
        if (unaryNode->Uop == UnaryOperator::Deref)
            return false;
        return IsDirectInStack(unaryNode->Lhs);
    }else if(auto binaryNode = std::dynamic_pointer_cast<BinaryNode>(node)){
        if (binaryNode ->Type->IsPointerType())
            return false;
       return IsDirectInStack(binaryNode->Lhs) && IsDirectInStack(binaryNode->Rhs);
    }
    return true;
}

//get the local variable  offset
int CodeGenerate::GetVarStackOffset(std::shared_ptr<AstNode> node){
    while (auto castNode = std::dynamic_pointer_cast<CastNode>(node)){
        node = castNode->CstNode;
    }
    if (auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(node)){
        return varExprNode->VarObj->Offset;
    }else if (auto constNode = std::dynamic_pointer_cast<ConstantNode>(node)){
        return constNode->Value;
    }else if(auto memberAccessNode = std::dynamic_pointer_cast<MemberAccessNode>(node)){
        auto record = std::dynamic_pointer_cast<RecordType>(memberAccessNode -> Lhs -> Type ->GetBaseType());
        auto field = record -> GetField(memberAccessNode -> fieldName);
        int _mOffset = GetVarStackOffset(memberAccessNode->Lhs);
        int _fOffset = field->Offset;
        return _mOffset + _fOffset;
    }else if(auto arrMemberNode = std::dynamic_pointer_cast<ArrayMemberNode>(node)){
        return GetVarStackOffset(arrMemberNode->Lhs) ;
    }else if(auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(node)){
        int _aOffset = GetVarStackOffset(unaryNode->Lhs);
        return _aOffset ;
    }else if(auto addNode = std::dynamic_pointer_cast<AddNode>(node)){
        int _lOffset = GetVarStackOffset(addNode->Lhs);
        int _rOffset = GetVarStackOffset(addNode->Rhs) ;
        if (addNode->Lhs->Type->IsArrayType())
            _rOffset *= addNode->Lhs->Type->GetBaseType()->Size;
        return _lOffset + _rOffset ;
    }else if(auto minusNode = std::dynamic_pointer_cast<MinusNode>(node)){
        int _lOffset = GetVarStackOffset(minusNode->Lhs);
        int _rOffset = GetVarStackOffset(minusNode->Rhs);
        return _lOffset - _rOffset ;
    }
    assert(0);
}

void CodeGenerate::GenerateAddress(AstNode *node) {
    GenerateAddress(node, false);
}

void CodeGenerate::GenerateAddress(AstNode *node,bool LValue) {
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
        }else if(varExprNode ->VarObj ->VarAttr->isStatic || varExprNode ->VarObj ->VarAttr->isGlobal){
            printf("\t  lea %s(%%rip),%s\n", varExprNode -> VarObj ->GlobalName.data(), GetCurTargetReg().data());
        }else if(varExprNode ->VarObj ->VarAttr->isReference){
            printf("\t  mov %d(%%rbp),%s\n", varExprNode -> VarObj -> Offset, GetCurTargetReg().data());
        }else{
            printf("\t  lea %d(%%rbp),%s\n", varExprNode -> VarObj -> Offset, GetCurTargetReg().data());
        }
    }else if(auto constNode = dynamic_cast<ConstantNode *>(node)){
        if (constNode->Type->IsPointerType()){
            printf("\t  mov $%s,%s\n",constNode->GetValue().data(),GetCurTargetReg().data());
        }else if (constNode->Type->IsBInType()){
            printf("\t  mov $%s,%s\n",constNode->GetValue().data(),GetCurTargetReg().data());
        }else{
            std::string constName =  std::string(constNode->Name);
            printf("\t  lea %s(%%rip),%s\n",constName.data(),GetCurTargetReg().data());
        }
    }else if (auto unaryNode = dynamic_cast<UnaryNode *>(node)){
        if (unaryNode -> Uop == UnaryOperator::Deref){
            Push(Type::VoidType);
            SetCurTargetReg("%rax");
            unaryNode ->Lhs->Accept(this);
            printf("\t  mov %%rax,%%rdi\n");
            PopCurTargetReg();
            Pop(Type::VoidType);
        }else if (unaryNode -> Uop == UnaryOperator::Addr){
            GenerateAddress(unaryNode ->Lhs.get());
        }else{
            printf("unaryNode must be defer!\n");
            assert(0);
        }
    }else if (auto memberAccessNode = dynamic_cast<MemberAccessNode *>(node)){
        auto record = std::dynamic_pointer_cast<RecordType>(memberAccessNode -> Lhs -> Type ->GetBaseType());
        GenerateAddress(memberAccessNode -> Lhs.get(),LValue);
        auto field = record -> GetField(memberAccessNode -> fieldName);
        printf("\t  add  $%d,%s\n", field ->Offset,GetCurTargetReg().data());
    }else if (auto arrayMemNode = dynamic_cast<ArrayMemberNode *>(node)){
        GenerateAddress(arrayMemNode->Lhs.get(),LValue);
    }else if (auto funcNode = dynamic_cast<FuncCallNode *>(node)){
        printf("\t  lea %d(%%rbp),%s\n",funcNode->ReturnStructOffset,GetCurTargetReg().data());
    }else if (auto incrNode = dynamic_cast<IncrNode *>(node)){
        GenerateAddress(incrNode->Lhs.get(),LValue);
    }else if (auto decrNode = dynamic_cast<DecrNode *>(node)){
        GenerateAddress(decrNode->Lhs.get(),LValue);
    }else if (auto binaryNode = dynamic_cast<BinaryNode *>(node)){
        SetCurTargetReg("%rax");
        node->Accept(this);
    }else{
        printf("not a value\n");
        assert(0);
    }
}

void CodeGenerate::Visitor(SizeOfExprNode *node) {
    printf("\t  mov $%d,%%rax   #sizeof %d\n",node -> Lhs -> Type  -> Size,node -> Lhs -> Type  -> Size);
}

void CodeGenerate::Visitor(DeclarationAssignmentStmtNode *node) {
    IsDeclaration = true;
    for (auto &n:node ->AssignNodes) {
        n ->Accept(this);
    }
    IsDeclaration = false;
}

void CodeGenerate::Load(AstNode *node) {
    auto type = node -> Type;
    if (auto castNode = dynamic_cast<CastNode *>(node)){
        type = castNode ->CstNode ->Type;
    }
    Load(type);
}

void CodeGenerate::Load(const std::shared_ptr<AstNode>& node) {
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
    }else if(type -> IsPointerType()){
        printf("\t  mov (%%rax),%s\n", GetRax(type->GetBaseType()).data());
    }else if( type -> IsRecordType() ){
    }else if( type -> GetBaseType()->IsArrayType()){
    }else if (type->IsArrayType() && type ->GetBaseType() ->IsBInType()){
        printf("\t  mov (%%rax),%s\n", GetRax(type ->GetBaseType()->Size).data());
    }else{
        printf("\t  mov (%%rax),%s\n", GetRax(type ->GetBaseType()->Size).data());
    }
}


bool  prevHandle(const std::shared_ptr<ConstantNode>& node){
    if (node ->Name != "" || node ->isRoot){
        PushStoreOffsetTag(node->Name);
        return true;
    }
    return false;
}


void HandleStore(std::shared_ptr<ConstantNode>& node, OffsetInfo * offset,
                 void ( * p)(std::shared_ptr<ConstantNode>& node, OffsetInfo * offset), bool PreNext){
    bool isPush = prevHandle(node);
    if (PreNext)
        node = node->Next;
    p(node,offset);
    if (isPush){
        PopStoreOffsetTag();
    }
}

int printBuildIn(std::shared_ptr<ConstantNode>& node){
    if (!node ->refStatic.empty()){
        printf("\t %s   %s\n", GetStoreCode(node ->Type ->Size).data(),std::string(node ->refStatic).data());
    }else{
        printf("\t %s   %s\n", GetStoreCode(node ->Type ->Size).data(),node->GetValue().data());
    }
    return node->Type->Size;
}


int printCharPointer(std::shared_ptr<ConstantNode>& node){
    auto cstNode = Scope::GetInstance() ->GetConstantTable().find(node->Name);
    if (cstNode!= Scope::GetInstance() ->GetConstantTable().end()){
        printf("\t .quad   %s\n",std::string(node->Name).data());
    }else{
        assert(0);
    }
    return 0;
}

std::string MergeCharArray(std::shared_ptr<ConstantNode>& node){
    auto cursor = node ->Next;
    std::string content;
    while(cursor){
        content += string_format("%c",cursor->Value);
        cursor = cursor ->Next;
    }
    return content;
}

void printPointer(std::shared_ptr<ConstantNode>& node){
    if (node->refStatic.size()){
        printf("\t .quad   %s\n",std::string(node->refStatic).data());
    }else{
        printf("\t .zero   %d\n",Type::VoidType->Size);
    }
}

int printString(std::shared_ptr<ConstantNode>& node){
    int size = node->Type->Size;
    std::string_view tk ;
    if (!node->Tk){
        node -> Tk = std::make_shared<Token>();
        auto ct = MergeCharArray(node);
        node -> Tk ->Content = ct;
        tk = ct;
    }else{
        tk = node->Tk->Content.substr(1,node->Tk->Content.size()-2);
    }
    std::string  storeCode;
    storeCode = size - tk.size() == 0 ? ".ascii": ".asciz";
    printf("\t %s  \"%s%s\"\n",storeCode.data(),std::string(tk).data(), RepeatN("\\000",size - tk.size() -1).data());
    node -> isStore = true;
    return 0;
}



int printArray(std::shared_ptr<ConstantNode>& node){
    auto cursor = node;
    auto aryType = node->Type;//array fisr element type as arrayType
    int (* p) (std::shared_ptr<ConstantNode>& node);
    if (cursor->Type->IsRecordType()){
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
    int offset = 0;
    while(cursor){
        if (cursor->isExpr){
            printf("\t %s ", GetStoreCode(node->Type->Size).data());
            offset += printExpr(cursor->Expr,0);
        }else{
            offset += p(cursor);
        }
        if (cursor)
            cursor = cursor->Next;
    }
    return offset;
}

int printStruct(std::shared_ptr<ConstantNode>& node){
    int offset = 0;
    while(node){
        if (node->Sub){
            offset = printStruct(node->Sub);
            break;
        }
        if (node->isRoot)
            node = node->Next;
        int gap = node->Offset - offset;
        printZero(gap);
        if (node->Type->IsRecordType()){
            printStruct(node);
        } else if (node->Type->IsPtrCharType()){
            printCharPointer(node);
        }else if (node->Type->IsStringType()){
            printString(node);
        }else if (node->Type->IsBInType()){
            printBuildIn(node);
        }else if (node->Type->IsArrayType()){
            printArray(node->Sub);
        }
        offset += node->Type->Size + gap;
        node = node ->Next;
    }
    if (node && node->Type->IsRecordType())
        printZero(node->Type->Size - offset);
    return offset;

}

void storeBuildIn(std::shared_ptr<ConstantNode>& node, OffsetInfo * offset){
    if (node->Type->IsIntegerNum() || node->Type->IsUnsignedNum()){
        if (node ->isStore){
            printf("\t  mov %%rax,%s\n",offset->GetOffset().data());
        }else{
            printf("\t  %s $%s,%s\n", GetMoveCode2(node->Type).data(),node->GetValue().data(),offset->GetOffset(node->Offset).data());
        }
    }else if(node-> Type-> IsFloatPointNum()){
        if (node ->isStore){
            printf("\t  mov %s(%%rip),%s\n", CurrentOffsetTag().data(),GetRax(node->Type).data());
            printf("\t  mov %s,%s\n", GetRax(node->Type).data(),offset->GetOffset(node->Offset).data());
        }else{
            printf("\t  mov $%s,%s\n", node->GetValue().data(),GetRax(node->Type).data());
            printf("\t  mov %s,%s\n", GetRax(node->Type).data(),offset->GetOffset(node->Offset).data());
        }
    }
}

void storePointer(std::shared_ptr<ConstantNode>& node, OffsetInfo * oi){
    if (node ->Type -> IsPtrCharType()){
        // handle cosnt char *
        printf("\t  lea %s(%%rip),%%rax\n", CurrentOffsetTag().data());
        if (node->Offset){
            printf("\t  add $%d,%%rax\n",node->Offset);
        }
        printf("\t  mov %%rax,%s\n", oi->GetOffset(node->Offset).data());
    }else{
        printf("\t  %s $%s,%s\n", GetMoveCode2(node->Type).data(), node->GetValue().data(), oi->GetOffset(node->Offset).data());
    }
}

void storeArray (std::shared_ptr<ConstantNode>&, OffsetInfo  * oi);
void storeHandle (std::shared_ptr<ConstantNode>&, OffsetInfo oi);

void storeString(std::shared_ptr<ConstantNode>& node, OffsetInfo * oi) {
    auto iter = Str2IntArrayIterator(node->Tk->Content);
    int offset = 0;
    while (iter.has_next()) {
        auto outPutIntNode = iter.next();
        if (node->isStore) {
            printf("\t  %s  %s(%%rip),%s\n",GetMoveCode2(outPutIntNode.Size).data(),node->Name.data(), GetRax(outPutIntNode.Size).data());
            printf("\t  %s  %s,(%%rdi)\n",GetMoveCode2(outPutIntNode.Size).data(), GetRax(outPutIntNode.Size).data());
        }else{
            printf("\t  %s $%lu,%s\n", GetMoveCode2(outPutIntNode.Size).data(), outPutIntNode.Value,
                   GetRax(outPutIntNode.Size).data());
        }
        printf("\t  %s %s,%s\n", GetMoveCode2(outPutIntNode.Size).data(), GetRax(outPutIntNode.Size).data(),
               oi -> GetOffset(offset).data());
        offset += outPutIntNode.Size;
    }
    node = nullptr;
}


void storeStruct(std::shared_ptr<ConstantNode>& node, OffsetInfo * offset) {
    while (node) {
        if (node->Type->IsBInType()){
            HandleStore(node,offset,storeBuildIn, false);
        }else if(node->Type->IsPointerType()){
            HandleStore(node,offset,storePointer, false);
        }else if(node->Type->IsRecordType()){
            HandleStore(node,offset, storeStruct, true);
        }else if(node->Type->IsStringType()){
            HandleStore(node,offset,storeString, false);
        }else if(node->Type->IsArrayType()){
            HandleStore(node,offset,storeArray, false);
        }
        if (node)
            node = node->Next;
        else
            break;
    }
}

void storeHandle(std::shared_ptr<ConstantNode>& node, OffsetInfo * oi) {
    if (node->isStore && node ->Type->Size >= 40){
        if (!oi->IsLoadAddTOReg()){
            printf("\t  lea %s,%%rdi\n",oi->GetOffset().data());
        }
        printf("\t  mov $%d,%%rcx\n",node -> Type ->Size);
        printf("\t  lea %s(%%rip),%%rsi\n",node -> Name.data());
        printf("\t  call _mempcy\n");
        return;
    }
    while (node) {
        if (node->Type->IsBInType()){
            HandleStore(node, oi, storeBuildIn, false);
        }else if(node->Type->IsPointerType()){
            HandleStore(node, oi, storePointer, false);
        }else if(node->Type->IsRecordType()){
            HandleStore(node, oi, storeStruct, true);
        }else if(node->Type->IsStringType()){
            HandleStore(node, oi, storeString, false);
        }else if(node->Type->IsArrayType()){
            HandleStore(node, oi, storeArray, false);
        }
        if (node)
            node = node->Next;
        else
            break;
    }
}

//use  rdx rax rdi copy array
void mmStoreCst(std::shared_ptr<ConstantNode>& node, OffsetInfo * oi, std::string name, bool isStore){
    int offset = node->Offset;
    std::shared_ptr<ConstantNode> cursor = node;
    if(!isStore){
        while(cursor){
            int size = 0;
            unsigned long fullNum = 0;
            while(size < 8){
                if ( size + cursor->Type->Size > 8)
                    break;
                if ( cursor->Type->Size != 8){
                    fullNum += atoll(cursor->GetValue().c_str()) << (size * 8);
                }else{
                    fullNum = atoll(cursor->GetValue().c_str());
                }
                size += cursor->Type->Size;
                cursor = cursor->Next;
                if (!cursor)
                    break;
            }
            if (size <=0)
                break;
            printf("\t  mov $%s,%s\n",  convert_to_hex(fullNum).data(), GetRax(size).data());
            printf("\t  mov %s,%s\n", GetRax(size).data(), oi->GetOffset(offset).data());
            fullNum = 0;
            offset += size;
        }
    }else{
        //if store init data in data region copy from data to target address
        printf("\t  lea %s(%%rip),%%rdx\n", name.data());
        while(cursor) {
            int size = 0;
            while (size < 8) {
                if (size + cursor->Type->Size > 8)
                    break;
                size += cursor->Type->Size;
                cursor = cursor->Next;
                if (!cursor)
                    break;
            }
            if (size <=0)
                break;
            printf("\t  %s %d(%%rdx),%s\n", GetMoveCode2(size).data(), offset , GetRax(size).data());
            printf("\t  %s %s,%s\n", GetMoveCode2(size).data(), GetRax(size).data(), oi->GetOffset(offset).data());
            offset += size;
        }
    }
    node = cursor;
}

void storeArray(std::shared_ptr<ConstantNode>& node, OffsetInfo * oi ){
    //todo array is struct
    mmStoreCst(node->Next,oi, node->Name, node->isStore);
}



void CodeGenerate::Store(const std::shared_ptr<AstNode>& node, OffsetInfo * oi) {
    std::shared_ptr<AstNode> cursor = node;
    std::shared_ptr<Type> type;
    while (auto castNode = std::dynamic_pointer_cast<CastNode>(cursor)){
        cursor = castNode ->CstNode;
    }
    if (auto constNode = std::dynamic_pointer_cast<ConstantNode>(cursor)){
            storeHandle(constNode,oi);
        return;
    }else if (auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(cursor)){
        type = varExprNode->Type;
    }else if(auto binaryNode = std::dynamic_pointer_cast<BinaryNode>(cursor)){
        type = binaryNode->Type;
    }else if(auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(cursor)){
        type = unaryNode->Type;
    }else if(auto funcCallNode = std::dynamic_pointer_cast<FuncCallNode>(cursor)){
        type = funcCallNode->Type;
    }else if(auto arrayMemberNode = std::dynamic_pointer_cast<ArrayMemberNode>(cursor)){
        type = arrayMemberNode->Type;
    }else if(auto ternaryNode = std::dynamic_pointer_cast<TernaryNode>(cursor)){
        type = ternaryNode->Type;
    }else if(auto memberAccessType = std::dynamic_pointer_cast<MemberAccessNode>(cursor)){
        type = memberAccessType->Type;
    }

    if (type -> IsPointerType()){
        printf("\t  mov %%rax,%s\n",oi->GetOffset().data());
        return;
    }else if(type->IsFloatPointNum()){
        printf("\t  %s %s,%s\n", GetMoveCode(type).data(),Xmm[Depth-1],oi->GetOffset().data());
        return;
    }else if((type->IsFunctionType() && type ->GetBaseType()->IsRecordType())
    || type ->IsRecordType()){
        if (IsDeclaration){
            return;
        }
        if (!oi->IsLoadAddTOReg())
            printf("\t  lea %s,%%rdi\n",oi->GetOffset().data());
        printf("\t  mov $%d,%%rcx\n",type->Size);
        printf("\t  call _mempcy\n");
        return;
    }else if(type ->IsArrayType()){
        printf("\t  mov %s,%s\n",GetRax(type).data(),oi->GetOffset().data());
        return;
    }else if(type ->IsFunctionType() && type ->GetBaseType()->IsBInType()){
        printf("\t  %s %s,%s\n", GetMoveCode2(node->Type).data(), GetRax(node->Type).data(),oi->GetOffset().data());
        return;
    }else{
        printf("\t  mov %s,%s\n",GetRax(type).data(),oi->GetOffset().data());
        return;
    }
    assert(0);
}


void CodeGenerate::Visitor(MemberAccessNode *node) {
    auto record = std::dynamic_pointer_cast<RecordType>(node ->Lhs ->Type ->GetBaseType());
    auto field = record -> GetField(node -> fieldName);
    if(IsDirectInStack(node->Lhs)){
        int offset = GetVarStackOffset(node->Lhs);
        printf("\t  mov  %d(%%rbp),%s\n", offset + field->Offset, GetRax(field->type).data());
    }else{
        GenerateAddress(node->Lhs.get());
        printf("\t  add  $%d,%%rax\n", field ->Offset);
        Load(field ->type);
    }
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


void CodeGenerate::PushStructOrUnion(const std::shared_ptr<AstNode>& node){
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
    }else if (castCode.empty()){
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
    if (ty ->IsRecordType() || ty ->IsPointerType() || ty ->IsArrayType() || ty->IsPtrCharType() ){
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
    if (ty ->IsRecordType() || ty ->IsPointerType() || ty ->IsArrayType() || ty->IsPtrCharType() ){
        size = Type::VoidType->Size;
    }
    printf("\t  sub $%d, %%rsp          #Push %s\n",size,GetRax(ty).data());
    printf("\t  mov %s,(%%rsp)\n",GetRax(ty).data());
    StackLevel ++;
}

void CodeGenerate::Pop(const std::shared_ptr<Type>& ty) {
    printf("\t  mov(%%rsp),%s           #Pop %s\n",GetRax(ty).data(),GetRax(ty).data());
    StackLevel --;
    if (ty->IsRecordType() || ty ->IsArrayType() || ty -> IsPointerType()){
        printf("\t  add $%d, %%rsp\n",Type::VoidType->Size);
        return;
    }
    printf("\t  add $%d, %%rsp\n",ty->Size);
}

void CodeGenerate::Pop(const std::shared_ptr<Type>& ty,const char *reg) {
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


void CodeGenerate::Visitor(ArrayMemberNode *node) {
    auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(node ->Lhs);
    if (IsDirectInStack(node->Lhs)){
        auto offset = GetVarStackOffset(node ->Lhs);
        printf("\t  mov %d(%%rbp),%s\n",offset, GetRax(node->Lhs->Type).data());
    }else{
        node ->Lhs->Accept(this);
    }
}

void CodeGenerate::Visitor(EmptyNode *node) {}


void CodeGenerate::Visitor(AssignNode *node) {
    UseXmm();
    SetAssignState();
    auto varExprNode = std::dynamic_pointer_cast<ExprVarNode>(node->Lhs);
    if (varExprNode && IsDeclaration)
        SetStructReturn2Offset(varExprNode->VarObj->Offset);
    auto constantNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    if (!constantNode){
        if (node->Rhs->Type->IsRecordType())
            SetCurTargetReg("%rsi");
        node -> Rhs -> Accept(this);
        //to handle a = b = 3 if rhs is b = 3 need to load b value
        if (auto rhs = std::dynamic_pointer_cast<AssignNode>(node->Rhs)){
            rhs->Lhs->Accept(this);
        }
        if (node->Rhs->Type->IsRecordType())
            PopCurTargetReg();
        if(auto funcCall = std::dynamic_pointer_cast<FuncCallNode>(node->Rhs)){
            if (funcCall->Type->GetBaseType()->IsRecordType()){
                SetCurTargetReg("%rsi");// when rhs is fun call and return struct eval rhs  and load rhs struct beginaddress
                GenerateAddress(node -> Rhs.get());
                ClearCurTargetReg();
            }
        }
    }
    SetStructReturn2Offset(0);
    ClearAssignState();
    OffsetInfo oi = *(OffsetInfo *)alloca(sizeof(OffsetInfo));
    //if var not in cur stack locad it's address to %rdi
    if (!IsDirectInStack(node->Lhs)){
        SetCurTargetReg("%rdi");
        GenerateAddress(node ->Lhs.get(), true);
        oi = OffsetInfo(GetCurTargetReg(),0, true);
        ClearCurTargetReg();
    }else {
        oi = OffsetInfo("%rbp", GetVarStackOffset(node->Lhs), false);
    }
    Store(node -> Rhs,&oi);
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
            if (!std::dynamic_pointer_cast<ConstantNode>(node -> Rhs)){
                printf("\t  imul $%d,%s\n", node ->Lhs-> Type ->GetBaseType() -> Size , GetRdi(node -> Rhs->Type).data());
            }
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
            if (!std::dynamic_pointer_cast<ConstantNode>(node -> Rhs)){
                printf("\t  imul $%d,%s\n", node -> Lhs-> Type->GetBaseType() -> Size , GetRdi(node -> Rhs->Type).data());
            }
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
    auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(node->Lhs);
    if (unaryNode && unaryNode->Uop == UnaryOperator::Deref){
        unaryNode ->Lhs->Accept(this);
        if (unaryNode->Type->IsFloatPointNum()){
            printf("\t  %s (%%rax),%s\n",  GetMoveCode2(constNode->Type).data(),Xmm[Depth++]);
            printf("\t  %s %s(%%rip),%s\n", GetMoveCode2(constNode->Type).data(),constNode->Name.data(),Xmm[Depth]);
            printf("\t  %s %s,%s\n", GetAdd(constNode->Type).data(),Xmm[Depth],Xmm[Depth-1]);
            Depth-=1;
        }else{
            printf("\t  mov (%%rax),%s\n", GetRcx(node -> Lhs ->Type).data());
            printf("\t  add $%s,%s\n",constNode->GetValue().data(), GetRcx(node -> Lhs ->Type).data());
        }
    }else{
        node -> Lhs -> Accept(this);
        if (node->Lhs->Type->IsPointerType()){
            printf("\t  lea %s(%%rax),%%rcx\n",constNode->GetValue().data());
        }else if(node->Lhs->Type->IsFloatPointNum()){
            printf("\t  %s %s(%%rip),%s\n", GetMoveCode2(constNode->Type).data(),constNode->Name.data(),Xmm[Depth]);
            printf("\t  %s %s,%s\n", GetAdd(constNode->Type).data(),Xmm[Depth],Xmm[Depth-1]);
            Depth-=1;
        }else{
            printf("\t  mov %s,%s\n", GetRax(node -> Lhs ->Type).data(), GetRcx(node -> Lhs ->Type).data());
            printf("\t  add $%s,%s\n",constNode->GetValue().data(), GetRcx(node -> Lhs ->Type).data());
        }
    }
    if (IsDirectInStack(node->Lhs)){
        if (node->Lhs->Type->IsFloatPointNum()){
            printf("\t  mov %s,%d(%%rbp)\n", Xmm[Depth], GetVarStackOffset(node->Lhs));
        }else{
            printf("\t  mov %s,%d(%%rbp)\n", GetRcx(node->Lhs->Type).data(), GetVarStackOffset(node->Lhs));
        }
    }else{
        if (node->Lhs->Type->IsFloatPointNum()){
            printf("\t  mov %s,(%%rax)\n",  Xmm[Depth]);
        }else {
            printf("\t  mov %s,(%%rax)\n", GetRcx(node->Lhs->Type).data());
        }
    }
}

void CodeGenerate::Visitor(DecrNode *node) {
    auto constNode = std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(node->Lhs);
    if (unaryNode && unaryNode->Uop == UnaryOperator::Deref){
        unaryNode ->Lhs->Accept(this);
        if (unaryNode->Type->IsFloatPointNum()){
            printf("\t  %s (%%rax),%s\n",  GetMoveCode2(constNode->Type).data(),Xmm[Depth++]);
            printf("\t  %s %s(%%rip),%s\n", GetMoveCode2(constNode->Type).data(),constNode->Name.data(),Xmm[Depth]);
            printf("\t  %s %s,%s\n", GetMinus(constNode->Type).data(),Xmm[Depth],Xmm[Depth-1]);
            Depth-=1;
        }else{
            printf("\t  mov (%%rax),%s\n", GetRcx(node -> Lhs ->Type).data());
            printf("\t  sub $%s,%s\n",constNode->GetValue().data(), GetRcx(node -> Lhs ->Type).data());
        }
    }else{
        node -> Lhs -> Accept(this);
        if (node->Lhs->Type->IsPointerType()){
            printf("\t  lea -%s(%%rax),%%rcx\n",constNode->GetValue().data());
        }else if(node->Lhs->Type->IsFloatPointNum()){
            printf("\t  %s %s(%%rip),%s\n", GetMoveCode2(constNode->Type).data(),constNode->Name.data(),Xmm[Depth]);
            printf("\t  %s %s,%s\n", GetMinus(constNode->Type).data(),Xmm[Depth],Xmm[Depth-1]);
            Depth-=1;
        }else{
            printf("\t  mov %s,%s\n", GetRax(node -> Lhs ->Type).data(), GetRcx(node -> Lhs ->Type).data());
            printf("\t  sub $%s,%s\n",constNode->GetValue().data(), GetRcx(node -> Lhs ->Type).data());
        }
    }
    if (IsDirectInStack(node->Lhs)){
        if (node->Lhs->Type->IsFloatPointNum()){
            printf("\t  mov %s,%d(%%rbp)\n", Xmm[Depth], GetVarStackOffset(node->Lhs));
        }else{
            printf("\t  mov %s,%d(%%rbp)\n", GetRcx(node->Lhs->Type).data(), GetVarStackOffset(node->Lhs));
        }
    }else{
        if (node->Lhs->Type->IsFloatPointNum()){
            printf("\t  mov %s,(%%rax)\n",  Xmm[Depth]);
        }else {
            printf("\t  mov %s,(%%rax)\n", GetRcx(node->Lhs->Type).data());
        }
    }
}

void CodeGenerate::Visitor(CmpNode *node) {
    if (node->Lhs->Type->IsFloatPointNum() || node->Rhs->Type->IsFloatPointNum()){
        node -> Lhs -> Accept(this);
        node -> Rhs -> Accept(this);
        if (node->BinOp ==BinaryOperator::FloatPointLesser || node->BinOp ==BinaryOperator::FloatPointLesserEqual){
            printf("\t  ucomiss %s, %s\n",Xmm[Depth - 2] ,Xmm[Depth - 1]);
            node->BinOp == BinaryOperator::FloatPointLesser ? node->BinOp =BinaryOperator::FloatPointGreater : node->BinOp =BinaryOperator::FloatPointGreaterEqual;
        }else if (node->BinOp ==BinaryOperator::FloatPointGreater || node->BinOp == BinaryOperator::FloatPointGreaterEqual){
            printf("\t  ucomiss %s, %s\n",Xmm[Depth - 1] ,Xmm[Depth - 2]);
        }else{
            printf("\t  ucomiss %s, %s\n",Xmm[Depth - 1] ,Xmm[Depth - 2]);
        }
        Depth -=2;
    }else{
        node -> Rhs -> Accept(this);
        Push(node -> Rhs->Type);
        node -> Lhs -> Accept(this);
        Pop(node->Rhs->Type, GetRdi(node->Rhs->Type).data());
        printf("\t  cmp %s,%s\n", GetRdi(node -> Rhs->Type).data(), GetRax(node -> Lhs->Type).data());
    }
    if(!IsCmpJmpModule){
        if (node->Rhs->Type->IsUnsignedNum() || node->Rhs->Type->IsPointerType()){
            printf("\t  %s  %%al\n", GetUnsignedSet(node -> BinOp).data());
            printf("\t  movzbl %%al,%%eax\n");
        }else{
            printf("\t  %s  %%al\n", GetSet(node -> BinOp).data());
            printf("\t  movzbl %%al,%%eax\n");
        }
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
        case BinaryOperator::BitShl:
            printf("\t  mov %%dil,%%cl\n");
            printf("\t  shl %%cl,%s\n",GetRax(node -> Lhs->Type).data());
            break;
        case BinaryOperator::BitShr:
            printf("\t  mov %%dil,%%cl\n");
            printf("\t  shr %%cl,%s\n",GetRax(node -> Lhs->Type).data());
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
    CmpZero(node->Cond);
    node ->Then->Accept(this);
    printf("\t  jmp .LT.end_%d\n",n);
    if (node ->Else){
        printf(".LT.else%d:\n",n);
        node ->Else->Accept(this);
    }
    printf(".LT.end_%d:\n",n);
    IsCmpJmpModule = false;
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


void CodeGenerate::PushJmpLabel(const std::string& labelName) {
    JmpStack.push_back(labelName);
}

std::string CodeGenerate::PopJmpLabel() {
    auto backLabel = GetJmpLabel();
    JmpStack.pop_back();
    return backLabel;
}

std::string CodeGenerate::GetJmpLabel() {
    auto backLabel = JmpStack.back();
    return std::string(backLabel);
}

std::string CodeGenerate::GetCurTargetReg() {
    if (curTargetReg.empty()){
        return "%rax";
    }
    return curTargetReg.back();
}

void CodeGenerate::SetCurTargetReg(const std::string& reg) {
    curTargetReg.push_back(reg);
}


void CodeGenerate::ClearCurTargetReg(){
    while (!curTargetReg.empty())
        curTargetReg.pop_back();
}
void CodeGenerate::PopCurTargetReg(){
        curTargetReg.pop_back();
}

int CodeGenerate::GetStructReturn2Offset() const {
    return Return2OffsetStack ;
}

void CodeGenerate::SetStructReturn2Offset(int offset) {
    Return2OffsetStack = offset;
}

void CodeGenerate::SetAssignState() {
    IsAssign = true;
}

void CodeGenerate::ClearAssignState() {
    IsAssign = false;
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

std::string OffsetInfo::GetOffset() {
    if (offset == 0)
        return string_format("(%s)",reg.data());
    return string_format("%d(%s)",offset,reg.data());
}

std::string OffsetInfo::GetOffset(int offset1) {
    int _offset = offset + offset1;
    if (_offset == 0)
        return string_format("(%s)",reg.data());
    return string_format("%d(%s)",_offset,reg.data());
}

bool OffsetInfo::IsLoadAddTOReg() const {
    return isLoad;
}
