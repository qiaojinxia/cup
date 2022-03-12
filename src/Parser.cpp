//
// Created by a on 2022/3/8.
//

#include "Parser.h"
#include "Lexer.h"
#include "AstNode.h"

using namespace BDD;

std::shared_ptr<AstNode> Parser::ParseAddExpr(std::shared_ptr<AstNode> left) {
    while (Lex.CurrentToken -> Kind == TokenKind::Add
    || Lex.CurrentToken -> Kind == TokenKind::Sub) {
        auto node = std::make_shared<BinaryNode>();
        BinaryOperator anOperator = BinaryOperator::Add;
        if(Lex.CurrentToken -> Kind == TokenKind::Sub)
            anOperator = BinaryOperator::Sub;
        Lex.GetNextToken();
        node -> BinOp = anOperator;
        node -> Lhs = left;
        node -> Rhs = ParseExpr();
        return node;
    }
    return nullptr;
}

std::shared_ptr<AstNode> Parser::ParseMultiExpr(std::shared_ptr<AstNode> left) {
    while (Lex.CurrentToken -> Kind == TokenKind::Mul
           || Lex.CurrentToken -> Kind == TokenKind::Div) {
        auto node = std::make_shared<BinaryNode>();
        BinaryOperator anOperator = BinaryOperator::Mul;
        if(Lex.CurrentToken -> Kind == TokenKind::Div)
            anOperator = BinaryOperator::Div;
        Lex.GetNextToken();
        node -> Lhs = left;
        node -> BinOp = anOperator;
        node -> Rhs = ParseExpr();
        return node;
    }
    return nullptr;
}

std::shared_ptr<AstNode> Parser::ParsePrimaryExpr() {
    if (Lex.CurrentToken -> Kind == TokenKind::LParen){
        Lex.GetNextToken();
        auto node = ParseExpr();
        Lex.GetNextToken();
        return node;
    }
    auto node = std::make_shared<ConstantNode>();
    node -> Value = Lex.CurrentToken -> Value;
    Lex.GetNextToken();
    return node;
}

std::shared_ptr<AstNode> Parser::ParseExpr() {
    std::shared_ptr<AstNode> left = ParsePrimaryExpr();
    auto node1 = ParseAddExpr(left);
    if (node1){
        return node1;
    }
    auto node2 = ParseMultiExpr(left);
    if (node2){
        return node2;
    }
    return left;
}

std::shared_ptr<ProgramNode> Parser::Parse() {
    auto node = std::make_shared<ProgramNode>();
    node -> Lhs = ParseExpr();
    return node;
}
