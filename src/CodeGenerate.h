//
// Created by a on 2022/3/8.
//

#ifndef BODDY_CODEGENERATE_H
#define BODDY_CODEGENERATE_H

#include "AstNode.h"

namespace BDD{
    class CodeGenerate:public AstVisitor{
    private:
        int StackLevel{0};
        int Sequence{0};
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
        void Push();
        void Pop(const char *reg);
    };
}


#endif //BODDY_CODEGENERATE_H
