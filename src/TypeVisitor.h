//
// Created by qiaojinxia on 2022/3/24.
//

#ifndef BODDY_TYPEVISITOR_H
#define BODDY_TYPEVISITOR_H
# include "AstNode.h"

namespace BDD{
    class TypeVisitor : public AstVisitor{
        void Visitor(ExprStmtNode *node) override;
        void Visitor(BinaryNode *node) override;
        void Visitor(ConstantNode *node) override;
        void Visitor(ExprVarNode *node) override;
        void Visitor(ProgramNode *node) override;
        void Visitor(IfElseStmtNode *node) override;
        void Visitor(SwitchCaseSmtNode *node) override;
        void Visitor(BlockStmtNode *node) override;
        void Visitor(WhileStmtNode *node) override;
        void Visitor(DoWhileStmtNode *node) override;
        void Visitor(ForStmtNode *node) override;
        void Visitor(FunctionNode *node) override;
        void Visitor(FuncCallNode *node) override;
        void Visitor(ReturnStmtNode *node) override;
        void Visitor(DeclarationStmtNode *node) override;
        void Visitor(DeclarationAssignmentStmtNode *node) override;
        void Visitor(StmtExprNode *node) override;
        void Visitor(UnaryNode *node) override;
        void Visitor(CastNode *node) override;
        void Visitor(SizeOfExprNode *node) override;
        void Visitor(MemberAccessNode *node) override;
        void Visitor(TernaryNode *node) override;
        void Visitor(BreakStmtNode *node) override;
        void Visitor(ContinueStmtNode *node) override;
        void Visitor(ArefNode *node) override;
        void Visitor(EmptyNode *node) override;

        void Visitor(AssignNode *node) override;
        void Visitor(AddNode *node) override;
        void Visitor(MinusNode *node) override;
        void Visitor(MulNode *node) override;
        void Visitor(DivNode *node) override;
        void Visitor(ModNode *node) override;

        void Visitor(IncrNode *node) override;
        void Visitor(DecrNode *node) override;
        void Visitor(CmpNode *node) override;
        void Visitor(BitOpNode *node) override;
        void Visitor(OrNode *node) override;
        void Visitor(AndNode *node) override;
    private:
        std::shared_ptr<Type> CurFuncType;
    public:
        std::shared_ptr<Type> CurAssignType;
    };

}


#endif //BODDY_TYPEVISITOR_H
