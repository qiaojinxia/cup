//
// Created by a on 2022/3/9.
//

#ifndef BODDY_PRINTVISITOR_H
#define BODDY_PRINTVISITOR_H

#include "AstNode.h"

namespace BDD{
    class PrintVisitor: public AstVisitor{
    public:
        PrintVisitor(){}
        void Visitor(ProgramNode *node) override;
    private:
        void Visitor(FunctionNode *node) override;
        void Visitor(ExprStmtNode *node) override;
        void Visitor(ExprVarNode *node) override;
        void Visitor(BinaryNode *node) override;
        void Visitor(ConstantNode *node) override;
        void Visitor(IfStmtNode *node) override;
        void Visitor(BlockStmtNode *node) override;
        void Visitor(WhileStmtNode *node) override;
        void Visitor(DoWhileStmtNode *node) override;
        void Visitor(ForStmtNode *node) override;
        void Visitor(FuncCallNode *node) override;
        void Visitor(ReturnStmtNode *node) override;
        void Visitor(DeclarationStmtNode *node) override;


    };


}
#endif //BODDY_PRINTVISITOR_H
