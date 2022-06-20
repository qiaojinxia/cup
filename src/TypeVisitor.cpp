//
// Created by qiaojinxia on 2022/3/24.
//

#include "TypeVisitor.h"
#include "Diag.h"
#include "Type.h"
#include "Scope.h"
#include "Common.h"

using namespace BDD;

void TypeVisitor::Visitor(ExprStmtNode *node) {
    if (node -> Lhs){
        node -> Lhs ->Accept(this);
        node -> Type = node -> Lhs ->Type;
    }
}

void TypeVisitor::Visitor(BinaryNode *node) {
    //to assume node type first
    auto bak = CurAssignType;
    CurAssignType = nullptr;
    node ->Lhs->Accept(this);
    node ->Rhs ->Accept(this);
    CurAssignType = bak;
    if (node->Lhs->Type->IsArrayType() && node->Rhs->Type->IsIntegerNum()){
        return;
    }
    //if lhs is 4 bytes rhs is 8 bytes convert lhs to 8bytes , always  convert to maxSize bytes size between lhs and rhs
    auto maxBitSize = node -> Lhs ->Type ->Size;
    if (node -> Rhs ->Type ->Size > maxBitSize){
        maxBitSize = node->Rhs->Type->Size;
    }

    //if express lhs or rhs is floatType ,convert  express to floatPoint operation
    bool hasFloatPoint = false;
    if (node->Lhs->Type->IsFloatPointNum() || node->Rhs->Type->IsFloatPointNum()){
        hasFloatPoint = true;
    }


    //if express lhs or rhs is unsigned ,convert express  to unsigned  operation
    bool hasUnsigned = false;
    if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()){
        hasUnsigned = true;
    }

    //if lhs and rhs less 4bytes convert to 4bytes
    if (maxBitSize < 4 && !hasUnsigned){
        node->Lhs = CastNodeType(node->Lhs->Type, Type::IntType, node->Lhs);
        node->Rhs = CastNodeType(node->Rhs->Type, Type::IntType, node->Rhs);
    }else if(maxBitSize < 4 && hasUnsigned){
        node->Lhs = CastNodeType(node->Lhs->Type, Type::UIntType, node->Lhs);
        node->Rhs = CastNodeType(node->Rhs->Type, Type::UIntType, node->Rhs);
    }else if (node -> Lhs -> Type -> IsArrayType() && node->Rhs->Type->IsIntegerNum()){
        node ->Type = Type::Pointer;
        return;
    }else if(node -> Lhs -> Type -> IsPointerType() && node->Rhs->Type->IsIntegerNum()){
        node ->Type = node->Lhs->Type;
        node->Rhs = CastNodeType(node->Rhs->Type,Type::LongType, node->Rhs);
        return;
    }else if(node -> Lhs -> Type -> IsLongType() && node->Rhs->Type->IsUIntType()){
        node ->Type = Type::LongType;
        node->Rhs = CastNodeType(node->Rhs->Type,Type::LongType, node->Rhs);
        return;
    }

    //auto convert if lhs type not equal to rhs type,set type to same
    if (node -> Lhs ->Type != node ->Rhs ->Type){
        if (hasFloatPoint){
            if (!(node->Lhs->Type->IsFloatPointNum() && node -> Lhs ->Type ->Size == maxBitSize)){
                node->Lhs = CastNodeType(node->Lhs->Type ,node->Rhs->Type,node->Lhs);
            }else if (!(node->Rhs->Type->IsFloatPointNum() && node -> Rhs ->Type ->Size == maxBitSize)){
                node->Rhs = CastNodeType(node->Rhs->Type ,node->Lhs->Type,node->Rhs);
            }
        }else {
                if (node->Lhs->Type->Size < maxBitSize || (hasUnsigned && node->Rhs->Type->IsUnsignedNum())){
                    node->Lhs = CastNodeType(node->Lhs->Type ,node->Rhs->Type,node->Lhs);
                }else if (node->Rhs->Type->Size < maxBitSize || (hasUnsigned && node->Lhs->Type->IsUnsignedNum())) {
                    node->Rhs = CastNodeType(node->Rhs->Type ,node->Lhs->Type,node->Rhs);
                }
        }
    }
    node ->Type = node->Lhs->Type;
}

void TypeVisitor::Visitor(ConstantNode *node) {
    auto cursor = node;
    if (CurAssignType){
        if (node->isRoot){
            node ->Type = CurAssignType;
            cursor = cursor->Next.get();
        }
        if (auto structType = std::dynamic_pointer_cast<RecordType>(CurAssignType)){
            for (auto &filed:structType->fields) {
                cursor -> Offset = filed ->Offset;
                cursor ->Type = filed ->type;
                if (cursor ->Sub != nullptr){
                    auto bak = CurAssignType;
                    CurAssignType = cursor ->Type;
                    cursor -> Sub ->Accept(this);
                    CurAssignType = bak;
                }
                cursor  = cursor ->Next.get();
                if (cursor == nullptr){
                    break;
                }
            }
            return;
        }else if(auto arrType = std::dynamic_pointer_cast<ArrayType>(CurAssignType)){
            if (arrType ->IsStringType()){
                cursor = cursor->Next.get();
            }
            int offset = 0;
            while (cursor) {
                cursor -> Offset = offset;
                offset += arrType->ElementType->Size;
                cursor ->Type = arrType->ElementType;
                if (cursor ->Next == nullptr){
                    break;
                }
                cursor  = cursor ->Next.get();
                if (cursor ->Sub != nullptr){
                    auto bak = CurAssignType;
                    cursor -> Sub ->Accept(this);
                    CurAssignType = bak;
                }

            }
            return;
        }
        if (node->Type != CurAssignType){
            auto _node  = CastNodeType(node->Type,CurAssignType, std::make_shared<ConstantNode>(* node));
            auto _node1 = std::dynamic_pointer_cast<ConstantNode>(_node);
            node->Value = _node1->Value;
            node->Type = _node1->Type;
        }

    }

}

void TypeVisitor::Visitor(ExprVarNode *node) {
    if (node ->VarObj){
        node -> Type = node ->VarObj ->Type;
    }
}

void TypeVisitor::Visitor(ProgramNode *node) {
    for(auto &fn:node ->Funcs){
        fn ->Accept(this);
    }
}

void TypeVisitor::Visitor(IfElseStmtNode *node) {
    node ->Cond ->Accept(this);
    if (node -> Else){
        node ->Then ->Accept(this);
    }
    if (node -> Else){
        node ->Else->Accept(this);
    }
}

void TypeVisitor::Visitor(BlockStmtNode *node) {
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
}

void TypeVisitor::Visitor(WhileStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Then ->Accept(this);
}

void TypeVisitor::Visitor(DoWhileStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Stmt ->Accept(this);
}

void TypeVisitor::Visitor(ForStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Stmt ->Accept(this);
    node ->Inc ->Accept(this);
    node ->Init ->Accept(this);
}

void TypeVisitor::Visitor(FunctionNode *node) {
    CurFuncType = node ->Type;
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
    CurFuncType = nullptr;
}


void TypeVisitor::Visitor(FuncCallNode *node) {
    CurAssignType = nullptr;
    auto funcType = std::dynamic_pointer_cast<FunctionType>(node->Type);
    int i = 0;
    for(auto &arg:node ->Args){
        if (funcType->Params[i]->Type != arg->Type){
            //convet paramtype
            auto _arg = CastNodeType(arg->Type, funcType->Params[i]->Type, arg);
            if (_arg){
                _arg->Accept(this);
                node ->Args[i] = _arg;
            }else{
//                assert(0);
            }
        }else{
            arg ->Accept(this);
        }
        i++;
    }
    if (node ->FuncPointerOffset)
        node ->FuncPointerOffset->Accept(this);
}

void TypeVisitor::Visitor(ReturnStmtNode *node) {
    if (auto ternaryNode = std::dynamic_pointer_cast<BDD::TernaryNode>(node->Lhs)){
        ternaryNode -> Type = node ->Type;
    }
    node ->Lhs ->Accept(this);
    auto lhsType =  node -> Lhs ->Type;
    if (auto funcType = std::dynamic_pointer_cast<FunctionType>(node->Lhs->Type))
        lhsType = funcType->ReturnType;
    if (lhsType != node ->Type ){
        //need to cast
        assert(0);
    }
    node ->Type =lhsType;
}

void TypeVisitor::Visitor(DeclarationStmtNode *node) {
    for(auto &n:node ->declarationNodes){
        n ->Accept(this);
    }
}

void TypeVisitor::Visitor(StmtExprNode *node) {
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
    if (!node->Stmts.empty())
        node ->Type = node ->Stmts .back()->Type;
}

void TypeVisitor::Visitor(UnaryNode *node) {
    node ->Lhs ->Accept(this);
    node -> Level = CurLevel;
    switch (node -> Uop) {
        case UnaryOperator::Plus:
        case UnaryOperator::Minus:
            node -> Type = node -> Lhs -> Type;
            break;
        case UnaryOperator::Deref:
            if (node -> Lhs -> Type ->IsPointerType()){
                node -> Type = node -> Lhs->Type->GetBaseType();
            }else if (node -> Lhs -> Type->IsArrayType()){
                node -> Type = std::dynamic_pointer_cast<ArrayType>(node -> Lhs->Type) ->ElementType;
            }else {
                node -> Type = node -> Lhs -> Type;
            }
            break;
        case UnaryOperator::Addr:
        {
            auto lhsUnaryNode = std::dynamic_pointer_cast<UnaryNode>(node->Lhs);
            //to handle &*(int *)0 convert to (int *)0
            if (lhsUnaryNode && lhsUnaryNode->Uop ==UnaryOperator::Deref){
                node->Lhs = lhsUnaryNode->Lhs;
            }
            node -> Type = std::make_shared<PointerType>(node -> Lhs ->Type);
        }
            break;
        case UnaryOperator::Decr:
        case UnaryOperator::Incr:
            node -> Type = node -> Lhs ->Type;
            break;
        case UnaryOperator::BitNot:
            node -> Type = node -> Lhs -> Type;
            break;
        case UnaryOperator::Not:
            node -> Type = Type::IntType;
            break;
    }
    if (node->Lhs->Type->IsFloatPointNum()){
        auto cstNode = std::make_shared<ConstantNode>(nullptr);
        cstNode ->Type = node->Lhs->Type;
        cstNode ->Value = 1;
        cstNode ->isModify = true;
        Scope::GetInstance()->PutToConstantTable(cstNode);
        node -> IncrOrDecrConstantTag = cstNode;
    }
}

void TypeVisitor::Visitor(SizeOfExprNode *node) {
        CurAssignType = nullptr;
        node -> Lhs ->Accept(this);
        node ->Type = node->Lhs->Type;
}

void TypeVisitor::Visitor(DeclarationAssignmentStmtNode *node) {
    for(auto &n:node ->AssignNodes){
        n ->Accept(this);
    }
    if (!node ->AssignNodes.empty())
        node ->Type = node -> AssignNodes.back()->Type;
}

void TypeVisitor::Visitor(MemberAccessNode *node) {
    node ->Lhs -> Accept(this);
    auto record = std::dynamic_pointer_cast<RecordType>(node ->Lhs  ->Type->GetBaseType());
    auto field = record ->GetField(node ->fieldName);
    node ->Type = field ->type;
}

void TypeVisitor::Visitor(BreakStmtNode *node) {}

void TypeVisitor::Visitor(ContinueStmtNode *node) {}

void TypeVisitor::Visitor(CastNode *node) {
    if (auto constNode = std::dynamic_pointer_cast<ConstantNode>(node ->CstNode)){
        constNode->Type = node ->Type;
    }
    node -> CstNode ->Accept(this);
}

void TypeVisitor::Visitor(ArrayMemberNode *node) {
    node -> Lhs ->Accept(this);
    node -> Type  = node ->Lhs ->Type->GetBaseType();
    if (CurAssignType && node -> Type != CurAssignType && CurAssignType ->IsBInType()){
        auto castNode = std::make_shared<CastNode>(nullptr);
        castNode ->Type = CurAssignType;
        castNode ->CstNode = std::make_shared<ArrayMemberNode>(*node);
    }
    CurAssignType = nullptr;
    node -> Offset ->Accept(this);
    //set the varName[index] index must greater eq then 4  to load by eax
    if (node ->Offset ->Type ->Size < Type::IntType->Size){
        auto castNode = std::make_shared<CastNode>(nullptr);
        castNode ->CstNode = node ->Offset;
        castNode ->Type = Type::IntType;
        node ->Offset= castNode;
    }
}

void TypeVisitor::Visitor(EmptyNode *node) {
    //do nothing
}

void TypeVisitor::Visitor(AssignNode *node) {
    node ->Lhs->Accept(this);
    //if rhs is constant set lhs type to constant node
    CurAssignType = node->Lhs->Type;
    if (CurAssignType ->IsAliasType()){
        CurAssignType = std::dynamic_pointer_cast<AliasType>(CurAssignType)->Base;
    }

    if(auto ternaryNode = std::dynamic_pointer_cast<TernaryNode>(node->Rhs)){
        ternaryNode ->Type = node->Lhs->Type;
    }
    node ->Rhs ->Accept(this);
    //cast rhs type same as lhs
    if (node ->Lhs ->Type != node ->Rhs ->Type  &&
        node -> Lhs -> Type -> Type::IsBInType()){
        auto castNode = std::make_shared<CastNode>(nullptr);
        castNode ->Type = node ->Lhs ->Type;
        castNode ->CstNode = node ->Rhs;
    }

    node ->Type = node ->Lhs->Type;
    CurAssignType = nullptr;
}


void TypeVisitor::Visitor(AddNode *node) {
    if (node -> isTypeInit){return;}
    node -> isTypeInit = true;
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = binary.Type;
    if (node -> Lhs -> Type ->IsFloatPointNum() || node->Rhs->Type->IsFloatPointNum()){
        node -> BinOp = BinaryOperator::FloatPointAdd;
    }else if (node -> Lhs -> Type ->IsPointerType() && (node->Rhs->Type->IsIntegerNum() || node->Rhs->Type->IsUnsignedNum()) ){
        node -> BinOp = BinaryOperator::PointerAdd;
        node ->Type = node -> Lhs -> Type;
    }else if (node -> Lhs -> Type -> IsArrayType() && (node->Rhs->Type->IsIntegerNum() || node->Rhs->Type->IsUnsignedNum()) ){
        node -> BinOp = BinaryOperator::PointerAdd;
        node ->Type = node -> Lhs -> Type;
    }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsArrayType()) {
        auto temp = node->Lhs;
        node->Lhs = node->Rhs;
        node->Rhs = temp;
        node->BinOp = BinaryOperator::PointerAdd;
        node->Type = node->Rhs->Type;
    }
}


void TypeVisitor::Visitor(MinusNode *node) {
    if (node -> isTypeInit){return;}
    node -> isTypeInit = true;
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = binary.Type;
    if(node -> Lhs -> Type -> IsPointerType() && node -> Rhs -> Type -> IsPointerType()){
        node -> BinOp = BinaryOperator::PointerDiff;
        node ->Type = Type::LongType;
    }else if (node ->Lhs ->Type ->IsFloatPointNum() || node ->Rhs->Type->IsFloatPointNum()){
        node -> BinOp = BinaryOperator::FloatPointMinus;
    }else if (node -> Lhs -> Type ->IsPointerType() && (node->Rhs->Type->IsIntegerNum() || node->Rhs->Type->IsUnsignedNum()) ){
        node -> BinOp = BinaryOperator::PointerSub;
        node ->Type = node -> Lhs -> Type;
    }else if (node -> Lhs -> Type -> IsArrayType() && (node->Rhs->Type->IsIntegerNum() || node->Rhs->Type->IsUnsignedNum()) ){
        node -> BinOp = BinaryOperator::PointerSub;
        node ->Type = std::make_shared<PointerType>(node -> Lhs -> Type->GetBaseType());
    }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsArrayType()) {
        auto temp = node->Lhs;
        node->Lhs = node->Rhs;
        node->Rhs = temp;
        node->BinOp = BinaryOperator::PointerSub;
        node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
    }
}

void TypeVisitor::Visitor(MulNode *node) {
    if (node -> isTypeInit){return;}
    node -> isTypeInit = true;
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = node ->Lhs ->Type;
    if (node -> Lhs ->Type ->IsFloatPointNum() || node -> Lhs ->Type ->IsFloatPointNum()){
        node ->BinOp = BinaryOperator::FloatPointMul;
    }
}

void TypeVisitor::Visitor(DivNode *node) {
    if (node -> isTypeInit){return;}
    node -> isTypeInit = true;
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    auto hasUnsigned = false;
    if (node->Lhs->Type->IsUnsignedNum()){
        hasUnsigned = true;
        node ->BinOp = BinaryOperator::Div;
    }else if (node->Lhs->Type->IsFloatPointNum() || node->Rhs->Type->IsFloatPointNum()){
        node ->BinOp = BinaryOperator::FloatPointDiv;
    }
    auto maxSize = node->Lhs->Type->Size;

    if (maxSize < node->Rhs->Type->Size ){
        if (node->Rhs->Type->Size < 4){
            auto castNode = std::make_shared<CastNode>(nullptr);
            if (hasUnsigned){
                castNode ->Type  = Type::UIntType;
            }else{
                castNode ->Type  = Type::IntType;
            }
            castNode ->CstNode = node->Rhs;
        }
    }else if (maxSize < 4){
        auto castNode = std::make_shared<CastNode>(nullptr);
        if (hasUnsigned){
            castNode ->Type  = Type::UIntType;
        }else{
            castNode ->Type  = Type::IntType;
        }
        castNode ->CstNode = node->Lhs;
    }
    node ->Type = node ->Lhs ->Type;
}


void TypeVisitor::Visitor(ModNode *node) {
    if (node -> isTypeInit){return;}
    node -> isTypeInit = true;
    auto divNode  = static_cast<DivNode>(*node);
    divNode.Accept(this);
    node ->Lhs = divNode.Lhs;
    node ->Rhs = divNode.Rhs;
    node ->Type = node ->Lhs ->Type;
    bool hasUnsigned = false;
    if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()) {
        hasUnsigned = true;
    }
    if (hasUnsigned){
        node ->BinOp = BinaryOperator::Mod;
    }else{
        node ->BinOp = BinaryOperator::IMod;
    }
    node->Type = node ->Lhs->Type;
}

void TypeVisitor::Visitor(IncrNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    auto ConstNode =  std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    ConstNode ->isModify = true;
    if (!node ->Lhs ->Type ->IsBInType() && !node->Lhs->Type->IsPointerType()){
        printf("IncrNode lhs error!");
        assert(0);
    }
    node ->Type = node ->Lhs->Type;
}

void TypeVisitor::Visitor(DecrNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    auto ConstNode =  std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    ConstNode ->isModify = true;
    if (!node ->Lhs ->Type ->IsBInType() && !node->Lhs->Type->IsPointerType()){
        printf("IncrNode lhs error!");
        assert(0);
    }
    node ->Type = node ->Lhs->Type;
}


void TypeVisitor::Visitor(CmpNode *node) {
    if (node -> isTypeInit){return;}
    node -> isTypeInit = true;
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = Type::IntType;
    if (node ->Lhs->Type->IsFloatPointNum() || node ->Lhs->Type->IsFloatPointNum()){
        if (node -> BinOp == BinaryOperator::Lesser){
            node -> BinOp = BinaryOperator::FloatPointLesser;
        }else if (node -> BinOp == BinaryOperator::LesserEqual){
            node -> BinOp = BinaryOperator::FloatPointLesserEqual;
        }else if (node -> BinOp == BinaryOperator::Greater){
            node -> BinOp = BinaryOperator::FloatPointGreater;
        }else if (node -> BinOp == BinaryOperator::GreaterEqual){
            node -> BinOp = BinaryOperator::FloatPointGreaterEqual;
        }else if (node -> BinOp == BinaryOperator::Equal){
            node -> BinOp = BinaryOperator::FloatPointEqual;
        }else if (node -> BinOp == BinaryOperator::NotEqual){
            node -> BinOp = BinaryOperator::FloatPointNotEqual;
        }else{
            assert(0);
        }
    }
}

void TypeVisitor::Visitor(BitOpNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    if (node->BinOp==BinaryOperator::BitSar && node ->Lhs->Type->IsUnsignedNum()){
        node->BinOp=BinaryOperator::BitShr;
    }else if(node->BinOp==BinaryOperator::BitSal && node ->Lhs->Type->IsUnsignedNum()){
        node->BinOp=BinaryOperator::BitShr;
    }
    node ->Type = node ->Lhs ->Type;
}

void TypeVisitor::Visitor(TernaryNode *node) {
    if ( node -> isTypeInit)
        return;
    node -> isTypeInit = true;
    node ->Cond ->Accept(this);
    node ->Then ->Accept(this);
    if (CurAssignType && node ->Then ->Type != CurAssignType){
        auto castNode = std::make_shared<CastNode>(nullptr);
        castNode -> Type = CurAssignType;
        castNode ->CstNode = node ->Then;
        node ->Then = castNode;
    }
    node ->Else ->Accept(this);
    if (CurAssignType && node ->Else ->Type != CurAssignType){
        auto castNode = std::make_shared<CastNode>(nullptr);
        castNode -> Type = CurAssignType;
        castNode ->CstNode = node ->Else;
        node ->Else = castNode;
    }
    if(CurAssignType){
        node  ->Type = CurAssignType;
    }else{
        node ->Type = node->Then->Type;
    }
}

void TypeVisitor::Visitor(SwitchCaseSmtNode *node) {
    node ->Value ->Accept(this);
    for (auto &branch:node->CaseBranch){
        branch.first->Accept(this);
        for (auto &statement:branch.second){
            statement ->Accept(this);
        }
    }
}

void TypeVisitor::Visitor(OrNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    node ->Type = Type::BoolType;
}

void TypeVisitor::Visitor(AndNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    node ->Type = Type::BoolType;
}


bool Type::IsTypeEqual(std::shared_ptr<Type>  tp1,std::shared_ptr<Type>  tp2){
    if (tp1 -> IsAliasType() && tp2 -> IsAliasType()){
        if (tp1->GetBaseType()  ==  tp2->GetBaseType() )
            return true;
    }else if(tp1 -> IsPointerType() && tp2 ->IsPointerType()){
        return true;

    }
    return true;
}

bool Type::IsTypeCanConvert(std::shared_ptr<Type>  tp1,std::shared_ptr<Type>  tp2){
    if (tp1 -> IsBInType() && tp2 ->IsBInType()){
       return true;
    }
    return false;
}