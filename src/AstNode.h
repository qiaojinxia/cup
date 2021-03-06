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
    class FuncCallNode;
    class ExprVarNode;
    class ReturnStmtNode;
    class BlockStmtNode;
    class ConstantNode;
    class AstNode {
    public:
        virtual ~AstNode() = default;
        explicit AstNode(std::shared_ptr<Token> tk):Tk(std::move(tk)){};
        virtual void Accept(AstVisitor *visitor) {};
        std::shared_ptr<Type> Type;
        std::shared_ptr<Token> Tk;
        int Level{0};
        bool isTypeInit{false};
    };

    class Attr{
    public:
        bool isInit{false};
        bool isReference{false};
        bool isStatic{false};
        bool isGlobal{false};
    };

    class Var{
    public:
        Var(std::shared_ptr<Type> type):Type(std::move(type)){VarAttr = std::make_shared<Attr>();}
        Var(std::shared_ptr<Type> type,std::string_view name):Type(std::move(type)),Name(name){VarAttr = std::make_shared<Attr>();}
    public:
        std::shared_ptr<Type> Type;
        std::string_view Name;
        std::string GlobalName;
        std::shared_ptr<Attr> VarAttr{};
        int Offset;
    };
    class ProgramNode:public  AstNode{
    public:
        explicit ProgramNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        void Accept(AstVisitor *visitor) override;
        std::list<std::shared_ptr<AstNode>> Funcs;
        std::list<std::shared_ptr<AstNode>> DeclarationNode;
        std::list<std::shared_ptr<Var>> Global{};
    };

    class FunctionNode: public AstNode{
    public:
        explicit FunctionNode(std::shared_ptr<Token> tk):AstNode(std::move(tk)){};
        std::string_view FuncName;
        std::list<std::shared_ptr<Var>> Params;
        std::list<std::shared_ptr<Var>> Locals;
        std::list<std::shared_ptr<AstNode>> Stmts;
        std::list<std::shared_ptr<ReturnStmtNode>> ReturnStmts;
        std::map<std::string_view ,std::shared_ptr<ExprVarNode>> ReturnVarMap;
        std::list<std::shared_ptr<FuncCallNode>> InnerFunCallStmts; // Function calls inside functions
        void Accept(AstVisitor *visitor) override;
    };

    class DeclarationInfoNode: public AstNode{
    public:
        DeclarationInfoNode(std::shared_ptr<Token> tk) : AstNode(tk){};
        std::shared_ptr<Token> ID;
        std::shared_ptr<AstNode> Value;
        std::shared_ptr<Var> Var;
    };

    enum class UnaryOperator {
        Plus = 1,
        Minus = -1,
        Deref = 2,
        Addr = 3,
        BitNot = 4,
        Not = 5,
        Incr = 6,
        Decr = 7,

    };

    class UnaryNode : public AstNode{
    public:
        explicit UnaryNode(std::shared_ptr<Token> tk):AstNode(tk){};
        UnaryOperator Uop;
        std::shared_ptr<AstNode> Lhs;
        std::shared_ptr<ConstantNode> IncrOrDecrConstantTag;
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
        AddAssign,
        MinusAssign,
        MulAssign,
        DivAssign,
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
        BitShr,
        BitShl,
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
    enum class Associativity{
        LeftAssociative = 1,
        RightAssociative = 2
    };
    class OperationPriority {
    public:
        TokenKind symbol;
        int priority;
        Associativity associativity;
        OperationPriority(TokenKind sb,int pri,Associativity ass ):symbol(sb),priority(pri),associativity(ass){};
    };
    static std::map<TokenKind,OperationPriority> OperatorPriorityTable = {
            {TokenKind::Assign, OperationPriority(TokenKind::Assign,14,Associativity::RightAssociative)},
            {TokenKind::Plus,   OperationPriority(TokenKind::Plus,4,Associativity::LeftAssociative)  },
            {TokenKind::Minus,    OperationPriority(TokenKind::Minus, 4,Associativity::LeftAssociative)},
            {TokenKind::Slash,     OperationPriority(TokenKind::Slash, 3,Associativity::LeftAssociative)},
            {TokenKind::Asterisk,  OperationPriority(TokenKind::Asterisk, 3,Associativity::LeftAssociative)},
            {TokenKind::Mod,         OperationPriority(TokenKind::Mod,  3,Associativity::LeftAssociative)},
            {TokenKind::Equal,      OperationPriority(TokenKind::Equal, 7,Associativity::LeftAssociative)},
            {TokenKind::NotEqual,      OperationPriority(TokenKind::NotEqual, 7,Associativity::LeftAssociative)},
            {TokenKind::LesserEqual,      OperationPriority(TokenKind::LesserEqual, 6,Associativity::LeftAssociative)},
            {TokenKind::Lesser,      OperationPriority(TokenKind::Lesser, 6,Associativity::LeftAssociative)},
            {TokenKind::GreaterEqual,      OperationPriority(TokenKind::GreaterEqual, 6,Associativity::LeftAssociative)},
            {TokenKind::Greater,     OperationPriority(TokenKind::Greater,  6,Associativity::LeftAssociative)},
            {TokenKind::VerticalBar,  OperationPriority(TokenKind::VerticalBar, 10,Associativity::LeftAssociative)},
            {TokenKind::Sal,  OperationPriority(TokenKind::Sal, 5,Associativity::LeftAssociative)},
            {TokenKind::Sar,  OperationPriority(TokenKind::Sar, 5,Associativity::LeftAssociative)},
            {TokenKind::Caret,  OperationPriority(TokenKind::Caret, 9,Associativity::LeftAssociative)},
            {TokenKind::Amp,  OperationPriority(TokenKind::Amp, 8,Associativity::LeftAssociative)},
            {TokenKind::Eof,      OperationPriority(TokenKind::Eof,  13,Associativity::LeftAssociative)},
            {TokenKind::And,               OperationPriority(TokenKind::And,  11,Associativity::LeftAssociative)},
            {TokenKind::Or,                OperationPriority(TokenKind::Or,  12,Associativity::LeftAssociative)},

            {TokenKind::PlusAssign,         OperationPriority(TokenKind::PlusAssign, 14,Associativity::RightAssociative)},
            {TokenKind::MinusAssign,       OperationPriority(TokenKind::MinusAssign,  14,Associativity::RightAssociative)},
            {TokenKind::AsteriskAssign,    OperationPriority(TokenKind::AsteriskAssign,  14,Associativity::RightAssociative)},
            {TokenKind::SlashAssign,       OperationPriority(TokenKind::SlashAssign,  14,Associativity::RightAssociative)},

            {TokenKind::ModAssign,         OperationPriority(TokenKind::ModAssign,  14,Associativity::RightAssociative)}, //%=
            {TokenKind::AmpAssign,          OperationPriority(TokenKind::AmpAssign, 14,Associativity::RightAssociative)},  // &=
            {TokenKind::VerticalBarAssign,  OperationPriority(TokenKind::VerticalBarAssign, 14,Associativity::RightAssociative)}, // |=
            {TokenKind::SarAssign,         OperationPriority(TokenKind::SarAssign,  14,Associativity::RightAssociative)},// >>=
            {TokenKind::SalAssign,         OperationPriority(TokenKind::SalAssign,  14,Associativity::RightAssociative)}, // <<=
            {TokenKind::CaretAssign,         OperationPriority(TokenKind::CaretAssign,  14,Associativity::RightAssociative)}, // <<=
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
        std::shared_ptr<AstNode> Expr;
        std::string Name;
        bool isExpr;
        bool isRoot{};
        bool isStore{};
        int Offset{};
        std::string_view refStatic;
        unsigned long  Value{};
        bool isModify{};
        bool isChar{false};
        std::string GetValue();
        bool HasSetValue();
        void Accept(AstVisitor *visitor) override;
        template<typename T>
        T GetValueT();


        std::string GetValueStr(std::shared_ptr<BuildInType> tp, const char *s_num);

        void CastValue(std::shared_ptr<BuildInType> toType);
        template<typename T>
        T GetValue(std::shared_ptr<BuildInType> tp, const char *s_num);

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
        std::shared_ptr<ReturnStmtNode> ReturnNode;
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
        int ReturnOffset{0}; //if returnType is  struct that record the offset of rbp store the return Struct write address
        void Accept(AstVisitor *visitor) override;
        std::shared_ptr<ExprVarNode> ReturnVarExpr;
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

    class ArrayMemberNode : public AstNode{
    public:
        explicit ArrayMemberNode(std::shared_ptr<Token> tk): AstNode(std::move(tk)){};
        std::shared_ptr<AstNode> Lhs; //expr node
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
        bool isConstantAssign;
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
        int CurLevel= {0};
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
        virtual void Visitor(ArrayMemberNode *node){};
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
