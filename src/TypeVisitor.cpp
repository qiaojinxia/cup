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
    node -> Rhs -> Type =  node -> Lhs -> Type;
    switch (node ->BinOp) {
        case BinaryOperator::Add:
            if (node -> Lhs -> Type ->IsPointerType() && node -> Rhs -> Type ->IsIntegerType()){
                node -> BinOp = BinaryOperator::PointerAdd;
                node ->Type = Type::Pointer;
            }else if (node -> Lhs -> Type -> IsIntegerType() && node -> Rhs -> Type -> IsPointerType()){
                node -> BinOp = BinaryOperator::PointerAdd;
                node ->Type = Type::Pointer;
            }else if (node -> Type -> IsIntegerType() && node -> Rhs -> Type -> IsIntegerType()){
            }else if (node -> Lhs -> Type -> IsArrayType() && node -> Rhs -> Type -> IsIntegerType()){
                node -> BinOp = BinaryOperator::ArrayPointerAdd;
            }else if (node -> Lhs -> Type -> IsIntegerType() && node -> Rhs -> Type -> IsArrayType()) {
                auto temp = node->Lhs;
                node->Lhs = node->Rhs;
                node->Rhs = temp;
                node->BinOp = BinaryOperator::ArrayPointerAdd;
                node ->Type = Type::Pointer;
            }else if (node -> Lhs -> Type ->IsFloatType() && node -> Rhs -> Type ->IsFloatType()){
                if (node -> Lhs -> Type -> Size == 8 && node -> Lhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleAdd;
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatAdd;
                }else{
                    assert(0);
                }
            }else if (node -> Type -> IsFloatType() && node -> Rhs -> Type -> IsFloatType()){
                node -> BinOp = BinaryOperator::FloatAdd;
            }else{
                printf("invalid add operation");
                assert(0);
            }
            break;
        case BinaryOperator::Sub:
            if (node -> Lhs -> Type ->IsPointerType() && node -> Rhs -> Type ->IsIntegerType()){
                node -> BinOp = BinaryOperator::PointerSub;
                node ->Type = Type::Pointer;
            }else if (node -> Lhs -> Type -> IsIntegerType() && node -> Rhs -> Type -> IsIntegerType()){
            }else if(node -> Lhs -> Type -> IsPointerType() && node -> Rhs -> Type -> IsPointerType()){
                node -> BinOp = BinaryOperator::PointerDiff;
                node ->Type = Type::Pointer;
            }else if (node -> Lhs -> Type -> IsIntegerType() && node -> Rhs -> Type -> IsArrayType()) {
                auto temp = node->Lhs;
                node->Lhs = node->Rhs;
                node->Rhs = temp;
                node->BinOp = BinaryOperator::ArrayPointerSub;
                node ->Type = Type::Pointer;
            }else if (node -> Lhs -> Type ->IsFloatType() && node -> Rhs -> Type ->IsFloatType()){
                if (node -> Lhs -> Type -> Size == 8 && node -> Lhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleSub;
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatSub;
                }else{
                    assert(0);
                }
            }else{
                printf("invalid sub operation");
                assert(0);
            }
            break;
        case BinaryOperator::PointerAdd:
            node ->Type = Type::Pointer;
        case BinaryOperator::PointerSub:
            node ->Type = Type::Pointer;
        case BinaryOperator::Assign:
            if (node -> Lhs -> Type ->IsFloatType() && node -> Rhs -> Type ->IsFloatType()){
                if (node -> Lhs -> Type -> Size == 8 && node -> Lhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleAssign;
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatAssign;
                }else{
                    assert(0);
                }
            }
            break;
        case BinaryOperator::Mul:
            if (node -> Lhs -> Type ->IsFloatType() && node -> Rhs -> Type ->IsFloatType()){
                if (node -> Lhs -> Type -> Size == 8 && node -> Lhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleMul;
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatMul;
                }else{
                    assert(0);
                }
            }
            break;
        case BinaryOperator::Div:
            if (node -> Lhs -> Type ->IsFloatType() && node -> Rhs -> Type ->IsFloatType()){
                if (node -> Lhs -> Type -> Size == 8 && node -> Lhs -> Type -> Size == 8){
                    node ->BinOp = BinaryOperator::DoubleDiv;
                }else if (node -> Lhs -> Type -> Size == 4 && node -> Lhs -> Type -> Size == 4){
                    node ->BinOp = BinaryOperator::FloatDiv;
                }else{
                    assert(0);
                }
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
            if (node -> Lhs -> Type->IsPointerType()){
                node -> Type = std::dynamic_pointer_cast<PointerType>(node -> Lhs->Type)->Base;
            }else if (node -> Lhs -> Type->IsArrayType()){
                node -> Type = std::dynamic_pointer_cast<ArrayType>(node -> Lhs->Type) ->ElementType;
            }else {
                printf("invalid defer operation");
            }
            break;
        case UnaryOperator::Amp:
            node -> Type = std::make_shared<PointerType>(node -> Lhs ->Type);
            break;
    }
}

void TypeVisitor::Visitor(SizeOfExprNode *node) {
        node -> Lhs ->Accept(this);
        node -> Type = node -> Lhs ->Type;
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

