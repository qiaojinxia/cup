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
    while (Lex.CurrentToken -> Kind != TokenKind::Eof){
        node ->Funcs.push_back(ParseFunc());
    }
    return node;
}

std::shared_ptr<AstNode> Parser::ParseStatement() {
    if (Lex.CurrentToken -> Kind == TokenKind::If){
        auto node = std::make_shared<IfStmtNode>();
        Lex.GetNextToken();
        Lex.ExceptToken(TokenKind::LParen);
        node ->Cond = ParseExpr();
        Lex.ExceptToken(TokenKind::RParen);
        node -> Then = ParseStatement();
        if (Lex.CurrentToken -> Kind == TokenKind::Else){
            Lex.GetNextToken();
            node -> Else = ParseStatement();
        }
        return node;
    }else if (Lex.CurrentToken -> Kind == TokenKind::LBrace){
        auto node = std::make_shared<BlockStmtNode>();
        Lex.GetNextToken();
        while (Lex.CurrentToken->Kind != TokenKind::RBrace){
            node -> Stmts.push_back(ParseStatement());
        }
        Lex.ExceptToken(TokenKind::RBrace);
        return node;
    }else if (Lex.CurrentToken -> Kind == TokenKind::While){
        auto node = std::make_shared<WhileStmtNode>();
        Lex.GetNextToken();
        Lex.ExceptToken(TokenKind::LParen);
        node -> Cond = ParseExpr();
        Lex.ExceptToken(TokenKind::RParen);
        node -> Then = ParseStatement();
        return node;
    }else if (Lex.CurrentToken -> Kind == TokenKind::Do){
        auto node = std::make_shared<DoWhileStmtNode>();
        Lex.GetNextToken();
        node -> Stmt = ParseStatement();
        Lex.ExceptToken(TokenKind::While);
        Lex.ExceptToken(TokenKind::LParen);
        node -> Cond = ParseExpr();
        Lex.ExceptToken(TokenKind::RParen);
        return node;
    } else if (Lex.CurrentToken -> Kind == TokenKind::For){
        auto node = std::make_shared<ForStmtNode>();
        Lex.GetNextToken();
        Lex.ExceptToken(TokenKind::LParen);
        if (Lex.CurrentToken->Kind != TokenKind::Semicolon){
            node -> Init = ParseExpr();
            Lex.ExceptToken(TokenKind::Semicolon);
            if (Lex.CurrentToken -> Kind !=TokenKind::Semicolon)
                node -> Cond = ParseExpr();
            Lex.ExceptToken(TokenKind::Semicolon);
            if (Lex.CurrentToken -> Kind != TokenKind::RParen)
                node -> Inc = ParseExpr();
            Lex.ExceptToken(TokenKind::RParen);
            node -> Stmt = ParseStatement();
            return node;
        }
    }
    auto node = std::make_shared<ExprStmtNode>();
        if (Lex.CurrentToken -> Kind != TokenKind::Semicolon){
        node -> Lhs = ParseExpr();
    }
    Lex.ExceptToken(TokenKind::Semicolon);
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

std::shared_ptr<AstNode> Parser::ParseFunc() {
    auto node =std::make_shared<FunctionNode>();
    LocalVars = &node -> Locals;
    LocalsMap.clear();
    Lex.ExceptToken(TokenKind::Function);
    node -> FuncName = Lex.CurrentToken->Content;
    Lex.ExceptToken(TokenKind::Identifier);
    Lex.ExceptToken(TokenKind::LParen);
    if (Lex.CurrentToken -> Kind != TokenKind::RParen){
        auto token = Lex.CurrentToken;
        ParsePrimaryExpr();
        node -> Params.push_back(LocalsMap[token->Content]);
        while (Lex.CurrentToken -> Kind == TokenKind::Comma){
            Lex.GetNextToken();
            auto token = Lex.CurrentToken;
            ParsePrimaryExpr();
            node -> Params.push_back(LocalsMap[token->Content]);
        }
    }
    Lex.ExceptToken(TokenKind::RParen);
    Lex.ExceptToken(TokenKind::LBrace);
    while (Lex.CurrentToken -> Kind != TokenKind::RBrace){
        node -> Stmts.push_back(ParseStatement());
    }
    Lex.ExceptToken(TokenKind::RBrace);
    return node;
}


