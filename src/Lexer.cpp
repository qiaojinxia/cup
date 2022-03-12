//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cctype>
#include "Lexer.h"

void BDD::Lexer::GetNextChar() {
    if (Cursor == SourceCode.size()){
        CurChar = '\0';
        return;
    }
    CurChar = SourceCode[Cursor++];
}

void BDD::Lexer::GetNextToken() {
    while(isspace(CurChar)){
        GetNextChar();
    }
    TokenKind kind;
    int value = 0;
    int startPos = Cursor -1;
    if (CurChar == '\0'){
        kind = TokenKind::Eof;
    }else if(CurChar == '+'){
        kind = TokenKind::Add;
        GetNextChar();
    }else if(CurChar == '-'){
        kind = TokenKind::Sub;
        GetNextChar();
    }else if(CurChar == '*'){
        kind = TokenKind::Mul;
        GetNextChar();
    }else if(CurChar == '/'){
        kind = TokenKind::Div;
        GetNextChar();
    }else if(CurChar == '('){
        kind = TokenKind::LParen;
        GetNextChar();
    }else if(CurChar == ')'){
        kind = TokenKind::RParen;
        GetNextChar();
    }else if(isdigit(CurChar)){
        kind = TokenKind::Num;
        value = 0;
        do {
            value = value * 10 + CurChar - '0';
            GetNextChar();
        }while (isdigit(CurChar));
    }else{
        printf("not support %c\n",CurChar);
    }
    CurrentToken = std::make_shared<Token>();
    CurrentToken->Kind = kind;
    CurrentToken->Value = value;
    CurrentToken->Content = SourceCode.substr(startPos,Cursor - 1 -startPos);
}

