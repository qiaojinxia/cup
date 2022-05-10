//
// Created by a on 2022/3/8.
//

#ifndef BODDY_CODEGENERATE_H
#define BODDY_CODEGENERATE_H

#include <string>
#include "AstNode.h"
#include "Scope.h"


namespace BDD{
    class CodeGenerate:public AstVisitor{
        std::unordered_map< std::string, std::string> CastMap;
    private:
        int StackLevel{0};
        int Sequence{0};
        const char *Regx64[5][6] = {
                {"%dil","%sil","%dl","%cl" ,"%r8b","%r9b"},
                {"%di","%si","%dx","%cx" ,"%r8w","%r9w"},
                {"%edi","%esi","%edx","%ecx" ,"%r8d","%r9d"},
                {"","","","" ,"",""},
                {"%rdi","%rsi","%rdx","%rcx" ,"%r8","%r9"},
        };
        const int MaxBit {8};
        const char *Xmm[8] = {"%xmm0","%xmm1","%xmm2","%xmm3" ,"%xmm4","%xmm5","%xmm6" };
        int XmmCount{0};

        int XmmPrevDepth;
        int Depth{0};
        Scope * scope;

        std::string CurrentFuncName;
        std::list<std::string_view> BreakStack;
        std::list<std::string_view> ContinueStack;
    public:

        CodeGenerate(){
            Scope:Scope::GetInstance();
        }
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
        void Visitor(CastNode *node) override;
        void Visitor(SizeOfExprNode *node) override;
        void Visitor(MemberAccessNode *node) override;
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
        void Visitor(DecrNode *node)override;
        void Visitor(CmpNode *node)override;
        void Visitor(BitOpNode *node)override;

        void PushBreak(std::string_view label);
        void PopBreak();
        std::string_view currentBreakTarget();

        void PushContinue(std::string_view label);
        void PopContinue();
        std::string_view currentContinueTarget();

        void USeXmm();
        void ReleaseXmm();

        void Push(std::shared_ptr<Type> ty);
        void Pop(std::shared_ptr<Type> ty);
        void Pop(std::shared_ptr<Type> ty,const char *reg);

        void Load(std::shared_ptr<AstNode> node);
        void Load(AstNode *node);
        void Store(std::shared_ptr<AstNode> node);

        void GenerateAddress(AstNode *node,std::string targetReg);

        const std::string GetMoveCode(std::shared_ptr<Type> type);
        const std::string GetMoveCode2(std::shared_ptr<Type>  type);

        const std::string GetIDivCode(std::shared_ptr<Type> type);

        const std::string GetRax(std::shared_ptr<Type> type);
        const std::string GetRcx(std::shared_ptr<Type> type);
        const std::string GetRcx(int size);
        const std::string GetRax(int size);
        const std::string GetAdd(std::shared_ptr<Type> type);

        std::string GetCastCode(std::string fromTo);

        const std::string GetRdi(std::shared_ptr<Type> sharedPtr);

        const std::string GetDivCode(std::shared_ptr<Type> type);

        void Load(std::shared_ptr<Type> type);

        const std::string GetMoveCode(int size);

        const std::string GetMinus(std::shared_ptr<Type> type);

        const std::string GetMul(std::shared_ptr<Type> type);

        const std::string GetDiv(std::shared_ptr<Type> type);

        const std::string GetRdx(std::shared_ptr<Type> type);

        const std::string GetSet(BinaryOperator op);
    };
}


#endif //BODDY_CODEGENERATE_H
