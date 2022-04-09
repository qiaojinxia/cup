//
// Created by a on 2022/3/8.
//

#ifndef BODDY_CODEGENERATE_H
#define BODDY_CODEGENERATE_H

#include <string>
#include "AstNode.h"


namespace BDD{
    class CodeGenerate:public AstVisitor{
    private:
        int StackLevel{0};
        int Sequence{0};
        const char *Regx64[6] = {"%rdi","%rsi","%rdx","%rcx" ,"%r8","%r9"};
        const char *Regx32[6] = {"%edi","%esi","%edx","%ecx" ,"%r8d","%r9d"};
        const char *Regx16[6] = {"%di","%si","%dx","%cx" ,"%r8w","%r9w"};
        const char *Regx8[6] = {"%dil","%sil","%dl","%cl" ,"%r8b","%r9b"};
        int RegCursor{0};
        std::string CurrentFuncName;
        std::list<std::string_view> BreakStack;
        std::list<std::string_view> ContinueStack;
    public:
        CodeGenerate(){}
    private:
        void Visitor(ExprStmtNode *node) override;
        void Visitor(BinaryNode *node) override;
        void Visitor(ConstantNode *node) override;
        void Visitor(ExprVarNode *node) override;
        void Visitor(ProgramNode *node) override;
        void Visitor(IfStmtNode *node) override;
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
        void Visitor(SizeOfExprNode *node) override;
        void Visitor(MemberAccessNode *node) override;
        void Visitor(BreakStmtNode *node) override;
        void Visitor(ContinueStmtNode *node) override;

        void Push();

        void PushBreak(std::string_view label);
        void PopBreak();
        std::string_view currentBreakTarget();

        void PushContinue(std::string_view label);
        void PopContinue();
        std::string_view currentContinueTarget();


        void Load(std::shared_ptr<Type> ty);
        void Store(std::shared_ptr<Type> ty);

        void GenerateAddress(AstNode *node);

        void Pop(const char *reg);
    };
}


#endif //BODDY_CODEGENERATE_H
