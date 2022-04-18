//
// Created by a on 2022/3/9.
//

#ifndef BODDY_ASTNODE_H
#define BODDY_ASTNODE_H

#include <memory>
#include <list>
#include <string_view>
#include <vector>
#include "Type.h"
#include <map>
#include <string>

namespace BDD{
    int AlignTo(int size,int align);
    class AstVisitor;
    class Field;
    class AstNode {
    public:
        virtual ~AstNode(){};
        virtual void Accept(AstVisitor *visitor) {};
        std::shared_ptr<Type> Type;
    };
    class Var{
    public:
        std::shared_ptr<Type> Type;
        std::string_view Name;
        int Offset;
    };
    class ProgramNode:public  AstNode{
    public:
        void Accept(AstVisitor *visitor) override;
        std::list<std::shared_ptr<AstNode>> Funcs;
    };

    class FunctionNode: public AstNode{
    public:
        std::string_view FuncName;
        std::list<std::shared_ptr<Var>> Params;
        std::list<std::shared_ptr<Var>> Locals;
        std::list<std::shared_ptr<AstNode>> Stmts;

        void Accept(AstVisitor *visitor) override;
    };

    enum class UnaryOperator {
        Plus,
        Minus,
        Deref,
        Addr,
    };

    class UnaryNode : public AstNode{
    public:
        UnaryOperator Uop;
        std::shared_ptr<AstNode> Lhs;
        void Accept(AstVisitor *visitor) override;
    };
    enum class BinaryOperator{
        Add,
        Incr,
        Decr,
        PointerAdd,
        ArrayPointerAdd,
        ArrayPointerSub,
        FloatAdd,
        FloatSub,
        FloatMul,
        FloatDiv,
        DoubleAdd,
        DoubleSub,
        DoubleMul,
        DoubleDiv,
        DoubleAssign,
        Sub,
        PointerSub,
        PointerDiff,
        Mul,
        Div,
        Mod,
        And,
        Or,
        Xor,
        Sal,
        Sar,
        Assign,
        FloatAssign,
        Equal,
        NotEqual,
        Greater,
        GreaterEqual,
        Lesser,
        LesserEqual,
        Eof,
    };
    static std::map<BinaryOperator,int> OpPrecedence = {
        {BinaryOperator::Equal, 7},
        {BinaryOperator::NotEqual, 7},
        {BinaryOperator::Greater, 6},
        {BinaryOperator::GreaterEqual, 6},
        {BinaryOperator::Lesser, 6},
        {BinaryOperator::LesserEqual, 6},
        {BinaryOperator::Add, 4},
        {BinaryOperator::Sub, 4},
        {BinaryOperator::Mul, 3},
        {BinaryOperator::Div, 3},
        {BinaryOperator::Mod, 3},
        {BinaryOperator::Eof, 15},
    };
    static std::map<TokenKind,int> TOpPrecedence = {

            {TokenKind::Assign, 14},
            {TokenKind::Plus,     4},
            {TokenKind::Minus,    4},
            {TokenKind::Slash,    3},
            {TokenKind::Asterisk, 3},
            {TokenKind::Equal,     7},
            {TokenKind::NotEqual,     7},
            {TokenKind::LesserEqual,     6},
            {TokenKind::Lesser,     6},
            {TokenKind::GreaterEqual,     6},
            {TokenKind::Greater,     6},
            {TokenKind::VerticalBar, 10},
            {TokenKind::Sal, 5},
            {TokenKind::Sar, 5},
            {TokenKind::Caret, 9},
            {TokenKind::Amp, 8},
            {TokenKind::Eof,      13},
    };

class BinaryNode :public  AstNode{
    public:
        BinaryOperator BinOp;
        std::shared_ptr<AstNode> Lhs;
        std::shared_ptr<AstNode> Rhs;
        void Accept(AstVisitor *visitor) override;
    };

    class ConstantNode : public AstNode{
    private:
    public:
        ConstantNode(std::list<std::shared_ptr<Token>> tk):Tokens(tk){};
        ConstantNode(std::shared_ptr<Token> tk){Tokens.push_back(tk);};
        std::string Name;
        std::list<std::shared_ptr<Token>> Tokens;
        int Value;
        void Accept(AstVisitor *visitor) override;
    };

    class ExprStmtNode: public AstNode{
    public:
        std::shared_ptr<AstNode> Lhs;

        void Accept(AstVisitor *visitor) override;
    };

    class ExprVarNode:public AstNode{
    public:
        std::string_view Name;
        std::shared_ptr<Var> VarObj;
        int Offset;
        void Accept(AstVisitor *visitor) override;
    };

    class IfStmtNode :public AstNode{
    public:
        std::shared_ptr<AstNode> Cond{nullptr};
        std::shared_ptr<AstNode> Then{nullptr};
        std::shared_ptr<AstNode> Else{nullptr};

        void Accept(AstVisitor *visitor) override;
    };

    class WhileStmtNode :public AstNode{
    public:
        std::shared_ptr<AstNode> Cond{nullptr};
        std::shared_ptr<AstNode> Then{nullptr};
        void Accept(AstVisitor *visitor) override;
    };
    class BlockStmtNode :public AstNode{
    public:
        std::list<std::shared_ptr<AstNode>> Stmts;
        void Accept(AstVisitor *visitor) override;
    };

    class DoWhileStmtNode :public AstNode{
    public:
        std::shared_ptr<AstNode> Stmt{nullptr};
        std::shared_ptr<AstNode> Cond{nullptr};
        void Accept(AstVisitor *visitor) override;
    };
    class ForStmtNode : public AstNode{
    public:
        std::shared_ptr<AstNode> Init{nullptr};
        std::shared_ptr<AstNode> Cond{nullptr};
        std::shared_ptr<AstNode> Inc{nullptr};
        std::shared_ptr<AstNode> Stmt{nullptr};
        void Accept(AstVisitor *visitor) override;
    };

    class FuncCallNode:public AstNode{
    public:
        std::string_view FuncName;
        std::vector<std::shared_ptr<AstNode>> Args;

        void Accept(AstVisitor *visitor) override;
    };

    class ReturnStmtNode:public AstNode{
    public:
        std::shared_ptr<AstNode> Lhs;
        void Accept(AstVisitor *visitor) override;
    };

    class DeclarationStmtNode: public AstNode{
    public:
        std::list<std::shared_ptr<ExprVarNode>> declarationNodes;
        void Accept(AstVisitor *visitor) override;
    };

    class DeclarationAssignmentStmtNode: public AstNode{
    public:
        std::list<std::shared_ptr<BinaryNode>> AssignNodes;
        void Accept(AstVisitor *visitor) override;
    };

    class StmtExprNode : public AstNode{
    public:
        std::list<std::shared_ptr<AstNode>> Stmts;
        void Accept(AstVisitor *visitor) override;
    };

    class SizeOfExprNode : public  AstNode{
    public:
        std::shared_ptr<AstNode> Lhs {nullptr};
        void Accept(AstVisitor *visitor) override;
    };

    class MemberAccessNode : public AstNode{
    public:
        std::shared_ptr<AstNode> Lhs;
        std::string_view fieldName;
        void Accept(AstVisitor *visitor) override;
    };

    class BreakStmtNode : public AstNode{
        std::shared_ptr<AstNode> Lhs;
        void Accept(AstVisitor *visitor) override;
    };
    
    class ContinueStmtNode : public AstNode{
        void Accept(AstVisitor *visitor) override;
    };

    class ArefNode : public AstNode{
    public:
        std::shared_ptr<AstNode> Lhs; //expr node
        std::shared_ptr<AstNode> Offset ;
        void Accept(AstVisitor *visitor) override;
    };

    class AssignNode : public AstNode{
    public:
        std::shared_ptr<AstNode> Lhs;
        std::shared_ptr<AstNode> Rhs;
        void Accept(AstVisitor *visitor) override;
    };


    enum class CastOperator {
        Double,
        Float,
        Int,
        Long,
        Pointer,
    };

    class CastNode : public AstNode{
    public:
        CastOperator Cop;
        std::shared_ptr<AstNode> Node;
        void Accept(AstVisitor *visitor) override;
    };

    class AstVisitor{
    public:
        virtual ~AstVisitor(){};
        virtual void Visitor(ProgramNode *node){};
        virtual void Visitor(BinaryNode *node){};
        virtual void Visitor(ConstantNode *node){};
        virtual void Visitor(ExprStmtNode *node){};
        virtual void Visitor(ExprVarNode *node){};
        virtual void Visitor(IfStmtNode *node){};
        virtual void Visitor(BlockStmtNode *node){};
        virtual void Visitor(WhileStmtNode *node){};
        virtual void Visitor(DoWhileStmtNode *node){};
        virtual void Visitor(ForStmtNode *node){};
        virtual void Visitor(FunctionNode *node){};
        virtual void Visitor(FuncCallNode *node){};
        virtual void Visitor(ReturnStmtNode *node){};
        virtual void Visitor(DeclarationStmtNode *node){};
        virtual void Visitor(DeclarationAssignmentStmtNode *node){};
        virtual void Visitor(StmtExprNode *node){};
        virtual void Visitor(UnaryNode *node){};
        virtual void Visitor(CastNode *node){};
        virtual void Visitor(SizeOfExprNode *node){};
        virtual void Visitor(MemberAccessNode *node){};
        virtual void Visitor(BreakStmtNode *node){};
        virtual void Visitor(ContinueStmtNode *node){};
        virtual void Visitor(ArefNode *node){};

    };
}

#endif //BODDY_ASTNODE_H
