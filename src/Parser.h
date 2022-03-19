//
// Created by a on 2022/3/8.
//

#ifndef BODDY_PARSER_H
#define BODDY_PARSER_H

#include <unordered_map>
#include "Lexer.h"
#include "AstNode.h"

namespace BDD{
    class Parser{
    private:
        Lexer &Lex;
        std::list<std::shared_ptr<Var>> *LocalVars{nullptr};
        std::unordered_map<std::string_view,std::shared_ptr<Var>> LocalsMap;
    public:
        Parser(Lexer &Lex):Lex(Lex){}
        std::shared_ptr<ProgramNode> Parse();

    private:
        std::shared_ptr<AstNode> ParseStatement();
        std::shared_ptr<AstNode> ParseExpr();

        std::shared_ptr<AstNode> ParseFunc();

        std::shared_ptr<AstNode> ParseFuncCallNode();


        std::shared_ptr<AstNode> ParseBinaryExpr(std::shared_ptr<AstNode> left);
        std::shared_ptr<AstNode> ParsePrimaryExpr();

        std::shared_ptr<Var> FindLocalVar(std::string_view varName);
        std::shared_ptr<Var> NewLocalVar(std::string_view varName);
    };

}



#endif //BODDY_PARSER_H
