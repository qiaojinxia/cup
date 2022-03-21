#include <iostream>
#include <stdio.h>
#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include "CodeGenerate.h"

using namespace BDD;

const char *source = "int sum(int x,int m,int j){return x + m + j;}int fib(int n) { if(n <=1){return 1;}else{ return fib(n -1) + fib(n-2);}} int prog(){int a,b,c = 1; return sum(a,b,c); + fib(5);}";

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
    CodeGenerate visitor;

    auto root = parser.Parse();
    root -> Accept(&visitor);
}

int main(int argc,char *argv[]) {
    if (argc != 3) {
        testParser();
        exit(0);
    }
    char * code_file = argv[1];

    if (std::string(code_file) == "path"){
        FILE *fp = fopen(argv[2],"r");
        if (fp == nullptr){
            printf("file open failed :%s\n",argv[1]);
            return 0;
        }
        char buffer[1024 * 10];
        size_t len = fread(buffer,1,sizeof(buffer),fp);
        buffer[len] = '\0';
        source = buffer;
    }
    source = argv[2];

    Lexer lex(source);
    lex.GetNextToken();

    Parser parser(lex);
    CodeGenerate codeGen;

    auto root = parser.Parse();
    root -> Accept(&codeGen);
    return 0;
}
