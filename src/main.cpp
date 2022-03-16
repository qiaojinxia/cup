#include <iostream>
#include <stdio.h>
#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include "CodeGenerate.h"

using namespace BDD;

const char *source = " b = 0; for(i =0;i<10;i=i+1){ b = b + 1;}b; ";

void testLexer(){
    Lexer lexer(source);
    do {
        lexer.GetNextToken();
        std::cout << lexer.CurrentToken->Content << std::endl;
    }while (lexer.CurrentToken->Kind != BDD::TokenKind::Eof);
}
    void testParser(){
    Lexer lex(source);
    lex.GetNextToken();

    Parser parser(lex);
    PrintVisitor visitor;

    auto root = parser.Parse();
    root -> Accept(&visitor);
}

int main(int argc,char *argv[]) {
    if (argc != 2) {
        testParser();
        exit(0);
    }
    source = argv[1];

    Lexer lex(source);
    lex.GetNextToken();

    Parser parser(lex);
    CodeGenerate codeGen;

    auto root = parser.Parse();
    root -> Accept(&codeGen);
    return 0;
}
