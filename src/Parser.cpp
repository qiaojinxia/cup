//
// Created by a on 2022/3/8.
//

#include "Parser.h"
#include "Lexer.h"
#include "AstNode.h"

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
    auto node = ParseBinaryExpr(left);
    return node;
}

std::shared_ptr<ProgramNode> Parser::Parse() {
    auto node = std::make_shared<ProgramNode>();
    node -> Lhs = ParseExpr();
    return node;
}
