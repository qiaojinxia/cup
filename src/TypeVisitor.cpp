//
// Created by qiaojinxia on 2022/3/24.
//

#include "TypeVisitor.h"
#include "Diag.h"
#include "Type.h"
using namespace BDD;

void TypeVisitor::Visitor(ExprStmtNode *node) {
    if (node -> Lhs){
        node -> Lhs ->Accept(this);
        node -> Type = node -> Lhs ->Type;
    }
}

void TypeVisitor::Visitor(BinaryNode *node) {
    node ->Lhs->Accept(this);
    node ->Rhs ->Accept(this);
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

    //auto convert if lhs type not equal to rhs type,set type to same
    if (node -> Lhs ->Type != node ->Rhs ->Type){
        if (hasFloatPoint){
            if (!(node->Lhs->Type->IsFloatPointNum() && node -> Lhs ->Type ->Size == maxBitSize)){
                auto castNode = std::make_shared<CastNode>();
                castNode ->CstNode = node->Lhs;
                castNode ->Type = node->Rhs->Type;
                node->Lhs  = castNode;
            }else if (!(node->Rhs->Type->IsFloatPointNum() && node -> Rhs ->Type ->Size == maxBitSize)){
                auto castNode = std::make_shared<CastNode>();
                castNode ->CstNode = node->Rhs;
                castNode ->Type = node->Lhs->Type;
                node->Rhs  = castNode;
            }
        }else {
                if (node->Lhs->Type->Size < maxBitSize){
                    auto castNode = std::make_shared<CastNode>();
                    castNode ->CstNode = node->Lhs;
                    castNode ->Type = node->Rhs->Type;
                    node->Lhs  = castNode;
                }else if (node->Rhs->Type->Size < maxBitSize) {
                    auto castNode = std::make_shared<CastNode>();
                    castNode->CstNode = node->Rhs;
                    castNode->Type = node->Lhs->Type;
                    node->Rhs = castNode;
                }
        }
    }
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
        node ->Type = CurAssignType;
    }

}

void TypeVisitor::Visitor(ExprVarNode *node) {
    node -> Type = node ->VarObj ->Type;
    node ->Offset = node ->VarObj ->Offset;
}

void TypeVisitor::Visitor(ProgramNode *node) {
    for(auto &fn:node ->Funcs){
        fn ->Accept(this);
    }
}

void TypeVisitor::Visitor(IfStmtNode *node) {
    node ->Cond ->Accept(this);
    node ->Then ->Accept(this);
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
    for(auto &stmt:node ->Stmts){
        stmt ->Accept(this);
    }
}

void TypeVisitor::Visitor(FuncCallNode *node) {
    CurAssignType = nullptr;
    for(auto &arg:node ->Args){
        arg ->Accept(this);
    }
}

void TypeVisitor::Visitor(ReturnStmtNode *node) {
   node ->Lhs ->Accept(this);
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
    node ->Type = node ->Stmts .back()->Type;
}

void TypeVisitor::Visitor(UnaryNode *node) {
    node ->Lhs ->Accept(this);
    switch (node -> Uop) {
        case UnaryOperator::Plus:
        case UnaryOperator::Minus:
            node -> Type = node -> Lhs -> Type;
            break;
        case UnaryOperator::Deref:
            if (node -> Lhs -> Type ->IsPointerType()){
                node -> Type = std::dynamic_pointer_cast<PointerType>(node -> Lhs->Type)->Base;
            }else if (node -> Lhs -> Type->IsArrayType()){
                node -> Type = std::dynamic_pointer_cast<ArrayType>(node -> Lhs->Type) ->ElementType;
            }else {
                node -> Type = node -> Lhs -> Type;
            }
            break;
        case UnaryOperator::Addr:
            node -> Type = std::make_shared<PointerType>(node -> Lhs ->Type);
            break;
    }
}

void TypeVisitor::Visitor(SizeOfExprNode *node) {
        node -> Lhs ->Accept(this);
        node ->Type = Type::IntType;
}

void TypeVisitor::Visitor(DeclarationAssignmentStmtNode *node) {
    for(auto &n:node ->AssignNodes){
        n ->Accept(this);
    }
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

void TypeVisitor::Visitor(ArefNode *node) {
    node -> Lhs ->Accept(this);
    node -> Type  = node ->Lhs ->Type->GetBaseType();
    if (CurAssignType && node -> Type != CurAssignType && CurAssignType ->IsBInType()){
        auto castNode = std::make_shared<CastNode>();
        castNode ->Type = CurAssignType;
        castNode ->CstNode = std::make_shared<ArefNode>(*node);
    }
    CurAssignType = nullptr;
    node -> Offset ->Accept(this);
    //set the varName[index] index must greater eq then 4  to load by eax
    if (node ->Offset ->Type ->Size < Type::IntType->Size){
        auto castNode = std::make_shared<CastNode>();
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
    node ->Rhs ->Accept(this);
    //cast rhs type same as lhs
    if (node ->Lhs ->Type != node ->Rhs ->Type  &&
        node -> Lhs -> Type -> Type::IsBInType()){
        auto castNode = std::make_shared<CastNode>();
        castNode ->Type = node ->Lhs ->Type;
        castNode ->CstNode = node ->Rhs;
    }
    CurAssignType = nullptr;
}


void TypeVisitor::Visitor(AddNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    if (node -> Lhs -> Type ->IsFloatPointNum() || node->Rhs->Type->IsFloatPointNum()){
        node -> BinOp = BinaryOperator::FloatPointAdd;
    }else if(node -> Lhs -> Type ->IsPointerType() ){
        node -> BinOp = BinaryOperator::PointerAdd;
    }else if (node -> Lhs -> Type -> IsArrayType() && node->Rhs->Type->IsIntegerNum()){
        node -> BinOp = BinaryOperator::PointerAdd;
    }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsArrayType()) {
        auto temp = node->Lhs;
        node->Lhs = node->Rhs;
        node->Rhs = temp;
        node->BinOp = BinaryOperator::PointerAdd;
        node->Type = std::make_shared<PointerType>(node->Rhs->Type);
    }
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = node ->Lhs ->Type;
}


void TypeVisitor::Visitor(MinusNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    if(node -> Lhs -> Type -> IsPointerType() && node -> Rhs -> Type -> IsPointerType()){
        node -> BinOp = BinaryOperator::PointerDiff;
    }else if (node ->Lhs ->Type ->IsFloatPointNum() || node ->Rhs->Type->IsFloatPointNum()){
        node -> BinOp = BinaryOperator::FloatPointMinus;
    }else if (node -> Lhs -> Type ->IsPointerType() && node->Rhs->Type->IsIntegerNum()){
        node -> BinOp = BinaryOperator::PointerSub;
        node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
    }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsArrayType()) {
        auto temp = node->Lhs;
        node->Lhs = node->Rhs;
        node->Rhs = temp;
        node->BinOp = BinaryOperator::PointerSub;
        node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
    }
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = node ->Lhs ->Type;
}

void TypeVisitor::Visitor(MulNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    if (node -> Lhs ->Type ->IsFloatPointNum() || node -> Lhs ->Type ->IsFloatPointNum()){
        node ->BinOp = BinaryOperator::FloatPointMul;
    }
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = node ->Lhs ->Type;
}

void TypeVisitor::Visitor(DivNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    auto hasUnsigned = false;
    if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()){
        hasUnsigned = true;
        node ->BinOp = BinaryOperator::Div;
    }else if (node->Lhs->Type->IsFloatPointNum() || node->Rhs->Type->IsFloatPointNum()){
        node ->BinOp = BinaryOperator::FloatPointDiv;
    }
    auto maxSize = node->Lhs->Type->Size;

    if (maxSize < node->Rhs->Type->Size ){
        if (node->Rhs->Type->Size < 4){
            auto castNode = std::make_shared<CastNode>();
            if (hasUnsigned){
                castNode ->Type  = Type::UIntType;
            }else{
                castNode ->Type  = Type::IntType;
            }
            castNode ->CstNode = node->Rhs;
        }
    }else if (maxSize < 4){
        auto castNode = std::make_shared<CastNode>();
        if (hasUnsigned){
            castNode ->Type  = Type::UIntType;
        }else{
            castNode ->Type  = Type::IntType;
        }
        castNode ->CstNode = node->Lhs;
    }
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = node ->Lhs ->Type;
}


void TypeVisitor::Visitor(ModNode *node) {
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
    ConstNode ->isChange = true;
    if (!node ->Lhs ->Type ->IsBInType()){
        printf("IncrNode lhs error!");
        assert(0);
    }
    node ->Type = node ->Lhs->Type;
}

void TypeVisitor::Visitor(DecrNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    auto ConstNode =  std::dynamic_pointer_cast<ConstantNode>(node -> Rhs);
    ConstNode ->isChange = true;
    if (!node ->Lhs ->Type ->IsBInType()){
        printf("IncrNode lhs error!");
        assert(0);
    }
    node ->Type = node ->Lhs->Type;
}


void TypeVisitor::Visitor(CmpNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    if (node ->Lhs->Type->IsFloatPointNum() || node ->Lhs->Type->IsFloatPointNum()){
        if (node -> BinOp == BinaryOperator::Lesser){
            node -> BinOp = BinaryOperator::FloatPointLesser;
        }else if (node -> BinOp == BinaryOperator::LesserEqual){
            node -> BinOp = BinaryOperator::FloatPointLesserEqual;
        }else if (node -> BinOp == BinaryOperator::Greater){
            node -> BinOp = BinaryOperator::FloatPointGreater;
        }else if (node -> BinOp == BinaryOperator::GreaterEqual){
            node -> BinOp = BinaryOperator::FloatPointGreater;
        }else if (node -> BinOp == BinaryOperator::Equal){
            node -> BinOp = BinaryOperator::FloatPointEqual;
        }else if (node -> BinOp == BinaryOperator::NotEqual){
            node -> BinOp = BinaryOperator::FloatPointNotEqual;
        }else{
            assert(0);
        }
    }
    auto binary  = static_cast<BinaryNode>(*node);
    binary.Accept(this);
    node ->Lhs = binary.Lhs;
    node ->Rhs = binary.Rhs;
    node ->Type = Type::BoolType;
}

void TypeVisitor::Visitor(BitOpNode *node) {
    node ->Lhs ->Accept(this);
    node ->Rhs ->Accept(this);
    node ->Type = node ->Lhs ->Type;
}
