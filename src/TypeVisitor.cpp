//
// Created by qiaojinxia on 2022/3/24.
//

#include "TypeVisitor.h"
#include "Diag.h"

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
    node -> Type =  node -> Lhs -> Type;
    switch (node ->BinOp) {
        case BinaryOperator::Add:
            if (node -> Lhs -> Type ->IsPointerType() && node->Rhs->Type->IsIntegerNum()){
                node -> BinOp = BinaryOperator::PointerAdd;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsPointerType()){
                node -> BinOp = BinaryOperator::PointerAdd;
                node ->Type = std::make_shared<PointerType>(node->Lhs->Type);
            }else if (node->Type->IsIntegerNum() && node->Rhs->Type->IsIntegerNum()){
            }else if (node -> Lhs -> Type -> IsArrayType() && node->Rhs->Type->IsIntegerNum()){
                node -> BinOp = BinaryOperator::PointerAdd;
            }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsArrayType()) {
                auto temp = node->Lhs;
                node->Lhs = node->Rhs;
                node->Rhs = temp;
                node->BinOp = BinaryOperator::PointerAdd;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsFloatNum() && node->Rhs->Type->IsFloatNum()){
                if (node -> Lhs -> Type -> Size == 8 || node -> Rhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleAdd;
                    if (node -> Lhs -> Type -> Size == 4 ){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Lhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Lhs = castNode;
                    }else if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatAdd;
                }else{
                    assert(0);
                }
            }else if (node->Type->IsFloatNum() && node->Rhs->Type->IsFloatNum()){
                node -> BinOp = BinaryOperator::FloatAdd;
            }else if (node-> Lhs-> Type->IsULongType() || node->Rhs->Type->IsULongType()){
                if (!node -> Lhs ->Type->IsULongType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Lhs;
                    castNode ->Type = Type::ULongType;
                    node -> Lhs = castNode;
                }else  if(!node-> Rhs ->Type->IsULongType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Rhs;
                    castNode ->Type = Type::ULongType;
                    node -> Rhs = castNode;
                }
            }else if (node-> Lhs-> Type->IsUIntType() || node->Rhs->Type->IsUIntType()){
                if (!node -> Lhs ->Type->IsUIntType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Lhs;
                    castNode ->Type = Type::UIntType;
                    node -> Lhs = castNode;
                }else  if(!node-> Rhs ->Type->IsUIntType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Rhs;
                    castNode ->Type = Type::UIntType;
                    node -> Rhs = castNode;
                }
            }else if(node->Lhs->Type == node->Rhs->Type ){
            }else{
                printf("invalid add operation");
                assert(0);
            }
            break;
        case BinaryOperator::Sub:
            if (node -> Lhs -> Type ->IsPointerType() && node->Rhs->Type->IsIntegerNum()){
                node -> BinOp = BinaryOperator::PointerSub;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsIntegerNum() && node->Rhs->Type->IsIntegerNum()){
            }else if(node -> Lhs -> Type -> IsPointerType() && node -> Rhs -> Type -> IsPointerType()){
                node -> BinOp = BinaryOperator::PointerDiff;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsIntegerNum() && node -> Rhs -> Type -> IsArrayType()) {
                auto temp = node->Lhs;
                node->Lhs = node->Rhs;
                node->Rhs = temp;
                node->BinOp = BinaryOperator::PointerSub;
                node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
            }else if (node->Lhs->Type->IsFloatNum() && node->Rhs->Type->IsFloatNum()){
                if (node -> Lhs -> Type -> Size == 8 || node -> Rhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleSub;
                    if (node -> Lhs -> Type -> Size == 4 ){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Lhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Lhs = castNode;
                    }else if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }
            }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                node ->BinOp = BinaryOperator::FloatSub;
            }else if (node-> Lhs-> Type->IsULongType() || node->Rhs->Type->IsULongType()){
                if (!node -> Lhs ->Type->IsULongType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Lhs;
                    castNode ->Type = Type::ULongType;
                    node -> Lhs = castNode;
                }else  if(!node-> Rhs ->Type->IsULongType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Rhs;
                    castNode ->Type = Type::ULongType;
                    node -> Rhs = castNode;
                }
            }else if (node-> Lhs-> Type->IsUIntType() || node->Rhs->Type->IsUIntType()){
                if (!node -> Lhs ->Type->IsUIntType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Lhs;
                    castNode ->Type = Type::UIntType;
                    node -> Lhs = castNode;
                }else  if(!node-> Rhs ->Type->IsUIntType()){
                    auto castNode = std::make_shared<CastNode>();
                    castNode -> CstNode = node -> Rhs;
                    castNode ->Type = Type::UIntType;
                    node -> Rhs = castNode;
                }
            }else if(node->Lhs->Type == node->Rhs->Type ){
            }else{
                printf("invalid sub operation");
                assert(0);
            }
            break;
        case BinaryOperator::PointerAdd:
            node ->Type = std::make_shared<PointerType>(node->Rhs->Type);;
        case BinaryOperator::PointerSub:
            node ->Type = std::make_shared<PointerType>(node->Rhs->Type);
        case BinaryOperator::Assign:
            if (node->Lhs->Type->IsFloatNum() && node->Rhs->Type->IsFloatNum()){
                if (node -> Lhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleAssign;
                    if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }else if (node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatAssign;
                    if (node -> Rhs -> Type -> Size == 8){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::FloatType;
                        node -> Rhs = castNode;
                    }
                }
            }
            break;
        case BinaryOperator::Mul:
            if (node->Lhs->Type->IsFloatNum() && node->Rhs->Type->IsFloatNum()){
                if (node -> Lhs -> Type -> Size == 8 || node -> Rhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleMul;
                    if (node -> Lhs -> Type -> Size == 4 ){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Lhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Lhs = castNode;
                    }else if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatMul;
                }
            }else if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()){
            }else if(node->Lhs->Type == node->Rhs->Type ){
            }else{
                assert(0);
            }
            break;
        case BinaryOperator::IDiv:
            if (node->Lhs->Type->IsFloatNum() && node->Rhs->Type->IsFloatNum()){
                if (node -> Lhs -> Type -> Size == 8 && node -> Rhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleDiv;
                    if (node -> Lhs -> Type -> Size == 4 ){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Lhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Lhs = castNode;
                    }else if (node -> Rhs -> Type -> Size == 4){
                        auto castNode = std::make_shared<CastNode>();
                        castNode -> CstNode = node -> Rhs;
                        castNode ->Type = Type::DoubleType;
                        node -> Rhs = castNode;
                    }
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatDiv;
                }
            }else if (node->Lhs->Type->IsUnsignedNum() || node->Rhs->Type->IsUnsignedNum()){

                node ->BinOp = BinaryOperator::Div;
            }else if(node->Lhs->Type == node->Rhs->Type ){
            }else{
                assert(0);
            }
            break;
        default:
            break;
    }
}

void TypeVisitor::Visitor(ConstantNode *node) {

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
    auto record = std::dynamic_pointer_cast<RecordType>(node ->Lhs  ->Type);
    auto field = record ->GetField(node ->fieldName);
    node ->Type = field ->type;
}

void TypeVisitor::Visitor(BreakStmtNode *node) {}

void TypeVisitor::Visitor(ContinueStmtNode *node) {}

void TypeVisitor::Visitor(CastNode *node) {
    node -> CstNode ->Accept(this);
}

void TypeVisitor::Visitor(ArefNode *node) {
    node -> Lhs ->Accept(this);
    node -> Offset ->Accept(this);
    if (auto leftArrayType = std::dynamic_pointer_cast<ArrayType>(node -> Lhs->Type)){
        node -> Type = leftArrayType -> ElementType;
        return;
    }
    auto leftPtrBaseType = std::dynamic_pointer_cast<PointerType>(node -> Lhs->Type)->Base;
    node -> Type  = leftPtrBaseType;
}

