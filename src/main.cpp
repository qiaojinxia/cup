#include <iostream>
#include <stdio.h>
#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include "CodeGenerate.h"
#include "TypeVisitor.h"

using namespace BDD;

const char *source = "int prog(){return 3 + 2 - 3  * 4  + 5 ;}";

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
    auto fileType  = std::string(code_file);
    if (fileType == "path"){
        if (strlen(argv[2]) == 0){
            printf("please input file path \n");
            return 0;
        }
        FILE *fp = fopen(argv[2],"r");
        if (fp == nullptr){
            printf("file open failed :%s\n",argv[2]);
            return 0;
        }
        char buffer[1024 * 10];
        size_t len = fread(buffer,1,sizeof(buffer),fp);
        buffer[len] = '\0';
        source = buffer;
    }else{
        source = argv[2];
    }

    Lexer lex(source);
    lex.GetNextToken();

    Parser parser(lex);
    CodeGenerate codeGen;
    TypeVisitor typeVisitor;
    auto root = parser.Parse();
    root ->Accept(&typeVisitor);
    root -> Accept(&codeGen);
    return 0;
}
