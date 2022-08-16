//
// Created by a on 2022/3/8.
//

#ifndef BODDY_PARSER_H
#define BODDY_PARSER_H

#include <unordered_map>
#include "Lexer.h"
#include "AstNode.h"
#include "Scope.h"
#define  TokenEqualTo(x) Lex.CurrentToken -> Kind == TokenKind::x
#define  TokenNotEqualTo(x) Lex.CurrentToken -> Kind != TokenKind::x
#define NextToken Lex.GetNextToken();
#define ExceptToken(x) Lex.ExceptToken(TokenKind::x);
#define ASSERT(x) DiagLoc(Lex.SourceCode,Lex.GetLocation(),x);
#define StoreLex(x) auto x = Lex;
#define ResumeLex(x) Lex = x;
namespace BDD{
    class Parser{
    private:
        Lexer &Lex;
        std::list<std::shared_ptr<Var>> *LocalVars{};
        BinaryOperator LastOperation{BinaryOperator::Eof};
        std::shared_ptr<AstNode> VarStack;
        std::shared_ptr<Token> SymbolStack;
        std::list<std::shared_ptr<FuncCallNode>> CurFuncCall{};
        std::list<std::shared_ptr<ReturnStmtNode>> CurFuncReturnNode{};
        bool addToConstant{true};
    public:
        Parser(Lexer &Lex):Lex(Lex){}
        std::shared_ptr<ProgramNode> Parse();
    private:
        std::shared_ptr<AstNode> ParseStatement();

        std::shared_ptr<AstNode> ParseExpr();

        std::shared_ptr<AstNode> ParseFunc(std::shared_ptr<FunctionNode> node);

        std::shared_ptr<AstNode> ParseFuncCallNode();

        std::shared_ptr<Type> ParseDeclarationSpec(const std::shared_ptr<Attr>& attr);

        std::shared_ptr<Type> GenerateType(int baseType,bool isConstant) const;

        std::shared_ptr<DeclarationInfoNode> ParseDeclarator(std::shared_ptr<Type> baseType);

        std::shared_ptr<Type> ParseTypeSuffix(std::shared_ptr<Type> baseType);

        std::shared_ptr<AstNode> ParsePostFixExpr();

        std::shared_ptr<AstNode> ParsePostFixArray(std::shared_ptr<AstNode> left);

        std::shared_ptr<AstNode> ParsePrimaryExpr();

        std::shared_ptr<AstNode> ParseUnaryExpr();

        std::shared_ptr<Type> ParseUnionDeclaration();

        std::shared_ptr<Type> ParseStructDeclaration();

        std::shared_ptr<AstNode> ParseEnumDeclaration();

        std::shared_ptr<RecordType> ParseRecord(RecordType::TagKind recordeType,std::shared_ptr<std::string_view>& name);

        static std::shared_ptr<Var> FindLocalVar(std::string_view varName);

        std::shared_ptr<Var> NewLocalVar(std::string_view varName,std::shared_ptr<Type> type,std::shared_ptr<Attr> attr);

        std::shared_ptr<AstNode> ParseBinaryExpr(int priority);

        std::shared_ptr<AstNode> ParseBinaryOperationExpr(const std::shared_ptr<AstNode>& left,BinaryOperator op);

        std::shared_ptr<AstNode> ParseCastExpr();

        std::shared_ptr<ConstantNode> ParseInitListExpr();

        std::shared_ptr<AstNode> ParseDeclarationExpr();

        std::shared_ptr<AstNode> ParseTernaryExpr(std::shared_ptr<AstNode> condition);

        std::shared_ptr<AstNode> ParseSwitchCaseStmt();

        std::shared_ptr<AstNode> ParseIfElseStmt();

        std::shared_ptr<AstNode> ParseWhileStmt();

        std::shared_ptr<AstNode> ParseForStmt();

        std::shared_ptr<AstNode> ParseDoWhileStmt();

        std::shared_ptr<AstNode> ParseBreakStmt();

        std::shared_ptr<AstNode> ParseReturnStmt();

        std::shared_ptr<AstNode> ParseContinueStmt();

        std::shared_ptr<AstNode> ParseTypeDef();

        std::shared_ptr<AstNode> ParseBlock();

        std::shared_ptr<AstNode> ParseCompoundStmt();



        bool ParseExtern();

    private:
        bool IsTypeName() const;

        std::shared_ptr<ConstantNode> parseInitListExpr();

        static std::shared_ptr<ExprVarNode> GetVarExprNode(const std::shared_ptr<AstNode>& node);

        std::shared_ptr<AssignNode> Assign(std::shared_ptr<AstNode> left, const std::shared_ptr<AstNode>& right);

        bool IsConstant() const;

        std::shared_ptr<DeclarationInfoNode> ParseIdentifier(std::shared_ptr<Type> baseType);

        std::shared_ptr<FunctionNode> IsFunc();


    };

}



#endif //BODDY_PARSER_H
