#include <iostream>
#include <stdio.h>
#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include "CodeGenerate.h"

using namespace BDD;
const char *code = "    3 + 8 - 21 * 42 / 7";

void testLexer(){
    Lexer lexer(code);
    do {
        lexer.GetNextToken();
        std::cout << lexer.CurrentToken->Content << std::endl;
    }while (lexer.CurrentToken->Kind != BDD::TokenKind::Eof);
}
void testParser(){
    Lexer lex(code);
    lex.GetNextToken();

    Parser parser(lex);
    PrintVisitor visitor;

    auto root = parser.Parse();
    root -> Accept(&visitor);
}

int main(int argc,char *argv[]) {

    if (argc != 4){
        printf("please input parameter");
        return 0;
    }
    const char *source = argv[1];

    Lexer lex(source);
    lex.GetNextToken();

    Parser parser(lex);
    CodeGenerate codeGen;

    auto root = parser.Parse();
    root -> Accept(&codeGen);
    return 0;
}
