//
// Created by a on 2022/3/9.
//

#ifndef BODDY_ASTNODE_H
#define BODDY_ASTNODE_H

#include <memory>

namespace BDD{
    class AstVisitor;
    class AstNode {
    public:
        virtual ~AstNode(){};
        virtual void Accept(AstVisitor *visitor) {};

    };

    class ProgramNode:public  AstNode{
    public:
        std::shared_ptr<AstNode> Lhs;
        void Accept(AstVisitor *visitor) override;
    };
    enum class BinaryOperator{
        Add,
        Sub,
        Mul,
        Div
    };

    class BinaryNode :public  AstNode{
    public:
        BinaryOperator BinOp;
        std::shared_ptr<AstNode> Lhs;
        std::shared_ptr<AstNode> Rhs;
        void Accept(AstVisitor *visitor) override;
    };

    class ConstantNode : public AstNode{
    public:
        int Value;
        void Accept(AstVisitor *visitor) override;
    };

    class AstVisitor{
    public:
        virtual ~AstVisitor(){};
        virtual void Visitor(ProgramNode *node){};
        virtual void   Visitor(BinaryNode *node){};
        virtual void Visitor(ConstantNode *node){};

    };
}

#endif //BODDY_ASTNODE_H
