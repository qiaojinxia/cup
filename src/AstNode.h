//
// Created by a on 2022/3/9.
//

#ifndef BODDY_ASTNODE_H
#define BODDY_ASTNODE_H

#include <memory>
#include <list>
#include <string_view>
#include <utility>
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
        virtual ~AstNode() = default;
        explicit AstNode(std::shared_ptr<Token> tk):Tk(std::move(tk)){};
        virtual void Accept(AstVisitor *visitor) {};
        std::shared_ptr<Type> Type;
        std::shared_ptr<Token> Tk;
        bool isTypeInit{false};
    };

    class Attr{
    public:
        bool isInit{false};
        bool isPublic{false};
        bool isParam{false};
        bool isStatic{false};
    };

    class Var{
    public:
        std::shared_ptr<Type> Type;
        std::string_view Name;
        std::string GlobalName;
        std::shared_ptr<Attr> VarAttr;
        bool isPointer{false};
        int Offset;
    };
    class ProgramNode:public  AstNode{
    public:
        explicit ProgramNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
        std::list<std::shared_ptr<AstNode>> Funcs;
    };

    class FunctionNode: public AstNode{
    public:
        explicit FunctionNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
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
        BitNot,
        Not,
    };

    class UnaryNode : public AstNode{
    public:
        explicit UnaryNode(std::shared_ptr<Token> tk):AstNode(tk){};
        UnaryOperator Uop;
        std::shared_ptr<AstNode> Lhs;
        void Accept(AstVisitor *visitor) override;
    };
    enum class BinaryOperator{
        Add,
        Incr,
        Decr,
        PointerAdd,
        FloatPointAdd,
        FloatPointMinus,
        FloatPointMul,
        FloatPointDiv,
        Div,
        Minus,
        PointerSub,
        PointerDiff,
        Mul,
        IDiv,
        Mod,
        IMod,
        BitAnd,
        BitOr,
        BitXor,
        BitSal,
        BitSar,
        Assign,
        Equal,
        FloatPointEqual,
        NotEqual,
        FloatPointNotEqual,
        Greater,
        FloatPointGreater,
        GreaterEqual,
        FloatPointGreaterEqual,
        Lesser,
        FloatPointLesser,
        LesserEqual,
        FloatPointLesserEqual,
        Eof,
        And,
        Or,
    };
    static std::map<TokenKind,int> TopPrecedence = {

            {TokenKind::Assign, 14},
            {TokenKind::Plus,     4},
            {TokenKind::Minus,    4},
            {TokenKind::Slash,    3},
            {TokenKind::Asterisk, 3},
            {TokenKind::Mod,         3},
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
            {TokenKind::And,      11},
            {TokenKind::Or,      12},
    };

class BinaryNode :public  AstNode{
    public:
        explicit BinaryNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        BinaryOperator BinOp;
        std::shared_ptr<AstNode> Lhs;
        std::shared_ptr<AstNode> Rhs;
        void Accept(AstVisitor *visitor) override;
    };

    class ConstantNode : public AstNode{
    public:
        explicit ConstantNode(std::shared_ptr<Token> tk): AstNode(std::move(tk)){};
        std::shared_ptr<ConstantNode> Next;
        std::shared_ptr<ConstantNode> Sub;
        std::string Name;
        bool isRoot{};
        bool isStore{};
        int Offset{};
        unsigned long Value{};
        bool isChange{};
        bool isChar{false};
        std::string GetValue();
        bool HasSetValue();
        void Accept(AstVisitor *visitor) override;
    };



    class ExprStmtNode: public AstNode{
    public:
        explicit ExprStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Lhs;
        void Accept(AstVisitor *visitor) override;
    };

    class ExprVarNode:public AstNode{
    public:
        explicit ExprVarNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::string_view Name;
        std::shared_ptr<Var> VarObj;
        bool isParam{false};
        void Accept(AstVisitor *visitor) override;
    };

    class IfElseStmtNode : public AstNode{
    public:
        explicit IfElseStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Cond{nullptr};
        std::shared_ptr<AstNode> Then{nullptr};
        std::shared_ptr<AstNode> Else{nullptr};

        void Accept(AstVisitor *visitor) override;
    };


    class WhileStmtNode :public AstNode{
    public:
        explicit WhileStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Cond{nullptr};
        std::shared_ptr<AstNode> Then{nullptr};
        void Accept(AstVisitor *visitor) override;
    };
    class BlockStmtNode :public AstNode{
    public:
        explicit BlockStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::list<std::shared_ptr<AstNode>> Stmts;
        void Accept(AstVisitor *visitor) override;
    };

    class DoWhileStmtNode :public AstNode{
    public:
        explicit DoWhileStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Stmt{nullptr};
        std::shared_ptr<AstNode> Cond{nullptr};
        void Accept(AstVisitor *visitor) override;
    };
    class ForStmtNode : public AstNode{
    public:
        explicit ForStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Init{nullptr};
        std::shared_ptr<AstNode> Cond{nullptr};
        std::shared_ptr<AstNode> Inc{nullptr};
        std::shared_ptr<AstNode> Stmt{nullptr};
        void Accept(AstVisitor *visitor) override;
    };

    class FuncCallNode:public AstNode{
    public:
        explicit FuncCallNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::string_view FuncName;
        std::vector<std::shared_ptr<AstNode>> Args;
        int ReturnStructOffset;
        std::shared_ptr<AstNode> FuncPointerOffset ;
        void Accept(AstVisitor *visitor) override;
    };

    class ReturnStmtNode:public AstNode{
    public:
        explicit ReturnStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Lhs;
        void Accept(AstVisitor *visitor) override;
    };

    class TernaryNode: public AstNode{
    public:
        explicit TernaryNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Cond{nullptr};
        std::shared_ptr<AstNode> Then{nullptr};
        std::shared_ptr<AstNode> Else{nullptr};
        void Accept(AstVisitor *visitor) override;
    };

    class DeclarationStmtNode: public AstNode{
    public:
        explicit DeclarationStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::list<std::shared_ptr<ExprVarNode>> declarationNodes;
        void Accept(AstVisitor *visitor) override;
    };



    class StmtExprNode : public AstNode{
    public:
        explicit StmtExprNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::list<std::shared_ptr<AstNode>> Stmts;
        void Accept(AstVisitor *visitor) override;
    };

    class SizeOfExprNode : public  AstNode{
    public:
        explicit SizeOfExprNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Lhs {nullptr};
        void Accept(AstVisitor *visitor) override;
    };

    class MemberAccessNode : public AstNode{
    public:
        explicit MemberAccessNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Lhs;
        std::string_view fieldName;
        void Accept(AstVisitor *visitor) override;
    };

    class BreakStmtNode : public AstNode{
    public:
        explicit BreakStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };
    
    class ContinueStmtNode : public AstNode{
    public:
    public:
        explicit ContinueStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };

    class ArefNode : public AstNode{
    public:
        explicit ArefNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Lhs; //expr node
        std::shared_ptr<AstNode> Offset ;
        void Accept(AstVisitor *visitor) override;
    };


    class CastNode : public AstNode{
    public:
        explicit CastNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> CstNode;
        void Accept(AstVisitor *visitor) override;
    };



    //when parse no effect statement return empty node such as: typedef
    class EmptyNode : public AstNode{
    public:
        explicit EmptyNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };



    class AssignNode : public BinaryNode{
    public:
        explicit AssignNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };
    class AddNode : public BinaryNode{
    public:
        explicit AddNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };
    class MinusNode : public BinaryNode{
    public:
        explicit MinusNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };
    class MulNode : public BinaryNode{
    public:
        explicit MulNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };
    class DivNode : public BinaryNode{
    public:
        explicit DivNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };
    class ModNode : public DivNode{
    public:
        explicit ModNode(std::shared_ptr<Token> tk):DivNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };

    class IncrNode : public BinaryNode{
    public:
        explicit IncrNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };

    class DecrNode : public BinaryNode{
    public:
        explicit DecrNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };

    class CmpNode : public BinaryNode{
    public:
        explicit CmpNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };

    class BitOpNode : public BinaryNode{
    public:
        explicit BitOpNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };

    class DeclarationAssignmentStmtNode: public AstNode{
    public:
        explicit DeclarationAssignmentStmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::list<std::shared_ptr<AssignNode>> AssignNodes;
        void Accept(AstVisitor *visitor) override;
    };

    class AndNode : public BinaryNode{
    public:
        explicit AndNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };

    class OrNode : public BinaryNode{
    public:
        explicit OrNode(std::shared_ptr<Token> tk):BinaryNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
    };

    class SwitchCaseSmtNode: public AstNode{
    public:
        explicit SwitchCaseSmtNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Value;
        std::list<std::shared_ptr<AstNode>> DefaultBranch;
        std::map<std::shared_ptr<AstNode>,std::list<std::shared_ptr<AstNode>>> CaseBranch;
        void Accept(AstVisitor *visitor) override;
    };

    class AstVisitor{
    public:
        virtual ~AstVisitor()= default;;
        virtual void Visitor(ProgramNode *node){};
        virtual void Visitor(BinaryNode *node){};
        virtual void Visitor(ConstantNode *node){};
        virtual void Visitor(ExprStmtNode *node){};
        virtual void Visitor(ExprVarNode *node){};

        //statement
        virtual void Visitor(IfElseStmtNode *node){};
        virtual void Visitor(BlockStmtNode *node){};
        virtual void Visitor(WhileStmtNode *node){};
        virtual void Visitor(DoWhileStmtNode *node){};
        virtual void Visitor(ForStmtNode *node){};
        virtual void Visitor(SwitchCaseSmtNode *node){};

        virtual void Visitor(FunctionNode *node){};
        virtual void Visitor(FuncCallNode *node){};
        virtual void Visitor(ReturnStmtNode *node){};
        virtual void Visitor(TernaryNode *node){};
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
        virtual void Visitor(EmptyNode *node){};


        //BinaryNode
        virtual void Visitor(AssignNode *node){};
        virtual void Visitor(AddNode *node){};
        virtual void Visitor(MinusNode *node){};
        virtual void Visitor(MulNode *node){};
        virtual void Visitor(DivNode *node){};
        virtual void Visitor(ModNode *node){};
        virtual void Visitor(IncrNode *node){};
        virtual void Visitor(DecrNode *node){};
        virtual void Visitor(CmpNode *node){};
        virtual void Visitor(BitOpNode *node){};
        virtual void Visitor(AndNode *node){};
        virtual void Visitor(OrNode *node){};
    };
}

#endif //BODDY_ASTNODE_H
