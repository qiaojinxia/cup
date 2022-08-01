//
// Created by a on 2022/3/8.
//

#ifndef BODDY_CODEGENERATE_H
#define BODDY_CODEGENERATE_H

#include <string>
#include "AstNode.h"
#include "Scope.h"


namespace BDD{
    void PushStoreOffsetTag(std::string_view label);
    void PopStoreOffsetTag();
    std::string_view CurrentOffsetTag();
    static std::list<std::string_view> OffsetTag;
    class OffsetInfo{
    private:
        std::string reg;
        bool isLoad;
        int offset;
    public:
        OffsetInfo(std::string targetReg,int initOffset,bool ld):reg(targetReg),offset(initOffset),isLoad(ld){};
        std::string GetOffset();
        bool IsLoadAddTOReg() const;
        std::string GetOffset(int offset1);
    };
    class CodeGenerate:public AstVisitor{
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
        bool IsCmpJmpModule{false};
        std::string CurrentFuncName;
        std::list<std::string_view> BreakStack;
        std::list<std::string_view> JmpStack;
        std::list<std::string_view> ContinueStack;
        int Return2OffsetStack;
        bool IsDeclaration{false};
        bool IsAssign{false};
        std::list<std::string> curTargetReg ={};
    public:
        CodeGenerate(){
            Scope:Scope::GetInstance();
        }
    private:
        void Visitor(ExprStmtNode *node) override;
        void Visitor(BinaryNode *node) override;
        void Visitor(TernaryNode *node) override;
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
        void Visitor(BreakStmtNode *node) override;
        void Visitor(ContinueStmtNode *node) override;
        void Visitor(ArrayMemberNode *node) override;
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
        void Visitor(AndNode *node)override;
        void Visitor(OrNode *node)override;

        void PushBreak(std::string_view label);
        void PopBreak();
        std::string_view currentBreakTarget();

        void PushContinue(std::string_view label);
        void PopContinue();
        std::string_view currentContinueTarget();

        void UseXmm();
        void ReleaseXmm();
        std::string GetCurTargetReg();
        void SetCurTargetReg(const std::string& reg);
        void ClearCurTargetReg();

        void Push(std::shared_ptr<Type> ty);
        void Pop(const std::shared_ptr<Type>& ty);
        void Pop(const std::shared_ptr<Type>& ty,const char *reg);

        void Load(const std::shared_ptr<AstNode>& node);
        void Load(AstNode *node);
        void Store(const std::shared_ptr<AstNode>& node, OffsetInfo * offset);


        void GenerateAddress(AstNode *node);
        void GenerateAddress(AstNode *node,bool LValue);


        bool IsDirectInStack(std::shared_ptr<AstNode> node);

        void Load(std::shared_ptr<Type> type);



        void PushJmpLabel(const std::string& labelName);
        std::string PopJmpLabel();
        std::string GetJmpLabel();


        void CmpZero(std::shared_ptr<AstNode> node);

        const std::string GetReg(int size, int n);

        static void PushStructOrUnion(const std::shared_ptr<AstNode>& node);

        void Push(std::shared_ptr<Type> ty, const char *reg);

        int GetStructReturn2Offset() const;
        void SetStructReturn2Offset(int offset);

        int GetVarStackOffset(std::shared_ptr<AstNode> node);

        void SetAssignState();
        void ClearAssignState();

        void PopCurTargetReg();
    };
}


#endif //BODDY_CODEGENERATE_H
