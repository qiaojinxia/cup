//
// Created by a on 2022/3/8.
//

#ifndef BODDY_PARSER_H
#define BODDY_PARSER_H

#include "Lexer.h"
#include "AstNode.h"

namespace BDD{
    class Parser{
    private:
        Lexer &Lex;
    public:
        Parser(Lexer &Lex):Lex(Lex){}
        std::shared_ptr<ProgramNode> Parse();

    private:
        std::shared_ptr<AstNode> ParseExpr();
        std::shared_ptr<AstNode> ParseBinaryExpr(std::shared_ptr<AstNode> left);
        std::shared_ptr<AstNode> ParsePrimaryExpr();

    };

}



#endif //BODDY_PARSER_H
