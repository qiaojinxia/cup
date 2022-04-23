//
// Created by a on 2022/3/8.
//

#ifndef BODDY_PARSER_H
#define BODDY_PARSER_H

#include <unordered_map>
#include "Lexer.h"
#include "AstNode.h"
#include "Scope.h"

namespace BDD{
    class Parser{
    private:
        Lexer &Lex;
        std::list<std::shared_ptr<Var>> *LocalVars{nullptr};
        BinaryOperator LastOperation{BinaryOperator::Eof};
        std::shared_ptr<AstNode> VarStack;
        std::shared_ptr<Token> SymbolStack;
    public:
        Parser(Lexer &Lex):Lex(Lex){}
        std::shared_ptr<ProgramNode> Parse();
    private:
        std::shared_ptr<AstNode> ParseStatement();

        std::shared_ptr<AstNode> ParseExpr();

        std::shared_ptr<AstNode> ParseFunc();

        std::shared_ptr<AstNode> ParseFuncCallNode();

        std::shared_ptr<Type> ParseDeclarationSpec(std::shared_ptr<Type> baseType);

        std::shared_ptr<Type> ParseDeclarator(std::shared_ptr<Type> baseType,std::list<std::shared_ptr<Token>> *nameTokens);

        std::shared_ptr<Type> ParseTypeSuffix(std::shared_ptr<Type> baseType);

        std::shared_ptr<AstNode> ParsePostFixExpr();

        std::shared_ptr<AstNode> ParsePrimaryExpr();

        std::shared_ptr<AstNode> ParseUnaryExpr();

        std::shared_ptr<Type> ParseUnionDeclaration();

        std::shared_ptr<Type> ParseStructDeclaration();

        std::shared_ptr<RecordType> ParseRecord(RecordType::TagKind recordeType);

        std::shared_ptr<Var> FindLocalVar(std::string_view varName);

        std::shared_ptr<Var> NewLocalVar(std::string_view varName,std::shared_ptr<Type> type);

        std::shared_ptr<AstNode> ParseBinaryExpr(int priority);

        std::shared_ptr<AstNode> ParseBinaryOperationExpr(std::shared_ptr<AstNode> left,BinaryOperator op);

        std::shared_ptr<AstNode> ParseCastExpr();

        std::shared_ptr<AstNode> ParseDeclarationExpr();

    private:
        const bool IsTypeName();
    };

}



#endif //BODDY_PARSER_H
