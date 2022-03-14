//
// Created by a on 2022/3/8.
//

#include "Parser.h"
#include "Lexer.h"
#include "AstNode.h"
#include "Diag.h"

using namespace BDD;

std::shared_ptr<AstNode> Parser::ParseBinaryExpr(std::shared_ptr<AstNode> left) {
    auto node = std::make_shared<BinaryNode>();
    BinaryOperator anOperator ;
    switch (Lex.CurrentToken -> Kind) {
        case TokenKind::Add:
            anOperator = BinaryOperator::Add;
            break;
        case TokenKind::Sub:
            anOperator = BinaryOperator::Sub;
            break;
        case TokenKind::Mul:
            anOperator = BinaryOperator::Mul;
            break;
        case TokenKind::Div:
            anOperator = BinaryOperator::Div;
            break;
        case TokenKind::Assign:
            anOperator = BinaryOperator::Assign;
            break;
        case TokenKind::Mod:
            anOperator = BinaryOperator::Mod;
            break;
        case TokenKind::Greater:
            anOperator = BinaryOperator::Greater;
            break;
        case TokenKind::GreaterEqual:
            anOperator = BinaryOperator::GreaterEqual;
            break;
        case TokenKind::Lesser:
            anOperator = BinaryOperator::Lesser;
            break;
        case TokenKind::LesserEqual:
            anOperator = BinaryOperator::LesserEqual;
            break;
        case TokenKind::Equal:
            anOperator = BinaryOperator::Equal;
            break;
        case TokenKind::NotEqual:
            anOperator = BinaryOperator::NotEqual;
            break;
        default:
            return left;
    }
    Lex.GetNextToken();
    node -> Lhs = left;
    node -> BinOp = anOperator;
    node -> Rhs = ParseExpr();
    return node;
}

std::shared_ptr<AstNode> Parser::ParsePrimaryExpr() {
    auto node = std::make_shared<AstNode>();
    switch (Lex.CurrentToken -> Kind){
        case TokenKind::LParen:
        {
            Lex.GetNextToken();
            node = ParseExpr();
            Lex.ExceptToken(TokenKind::RParen);
            break;
        }
        case TokenKind::Identifier:
        {
            auto exprVarNode = std::make_shared<ExprVarNode>();
            exprVarNode -> Name = Lex.CurrentToken->Content;
            auto obj = FindLocalVar(Lex.CurrentToken -> Content);
            if (!obj){
                obj = NewLocalVar(Lex.CurrentToken -> Content);
            }
            exprVarNode ->VarObj = obj;
            node = exprVarNode;
            Lex.GetNextToken();
            break;
        }
       case TokenKind::Num:
       {
           auto constNode = std::make_shared<ConstantNode>();
           constNode -> Value = Lex.CurrentToken -> Value;
           Lex.GetNextToken();
           node =  constNode;
           break;
       }
       default:
           DiagE(Lex.SourceCode,Lex.CurrentToken->Location.Line,Lex.CurrentToken->Location.Col,"not support type");
    }
    return node;
}

std::shared_ptr<AstNode> Parser::ParseExpr() {
    std::shared_ptr<AstNode> left = ParsePrimaryExpr();
    auto node = ParseBinaryExpr(left);
    return node;
}

std::shared_ptr<ProgramNode> Parser::Parse() {
    auto node = std::make_shared<ProgramNode>();
    LocalVars = &node -> LocalVariables;
    while (Lex.CurrentToken -> Kind != TokenKind::Eof){
        node -> Statements.push_back(ParseStatement());
    }
    return node;
}

std::shared_ptr<AstNode> Parser::ParseStatement() {
    auto node = std::make_shared<ExprStmtNode>();
    node -> Lhs = ParseExpr();
    if (Lex.CurrentToken -> Kind != TokenKind::Semicolon){
        DiagE(Lex.SourceCode,Lex.CurrentToken->Location.Line,Lex.CurrentToken->Location.Col,"except ';'");
    }
    Lex.GetNextToken();
    return node;
}

std::shared_ptr<Var> Parser::FindLocalVar(std::string_view varName) {
    if (LocalsMap.find(varName) != LocalsMap.end()){
        return LocalsMap[varName];
    }
    return nullptr;
}

std::shared_ptr<Var> Parser::NewLocalVar(std::string_view varName) {
    auto obj = std::make_shared<Var>();
    obj ->Name = varName;
    obj -> Offset = 0;
    LocalVars -> push_front(obj);
    LocalsMap[varName] = obj;
    return obj;
}


