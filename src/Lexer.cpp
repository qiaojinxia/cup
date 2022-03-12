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
        if (CurChar == '\n'){
            Line ++;
            LineHead = Cursor;
        }
        GetNextChar();
    }
    SourceLocation Location;
    TokenKind kind;
    Location.Line = Line;
    Location.Col = Cursor - 1 -LineHead;
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
    }else if(CurChar == ';'){
        kind = TokenKind::Semicolon;
        GetNextChar();
    }else if(CurChar == '='){
        kind = TokenKind::Assign;
        GetNextChar();
    }else if(isdigit(CurChar)){
        kind = TokenKind::Num;
        value = 0;
        do {
            value = value * 10 + CurChar - '0';
            GetNextChar();
        }while (isdigit(CurChar));
    }else{
        if (IsLetter()){
            GetNextChar();
            while(IsLetterOrDigit()){
                GetNextChar();
            }
            kind = TokenKind::Identifier;
        }else{
            printf("not support %c\n",CurChar);
            assert(0);
        }
    }
    CurrentToken = std::make_shared<Token>();
    CurrentToken->Kind = kind;
    CurrentToken->Value = value;
    CurrentToken -> Location = Location;
    CurrentToken->Content = SourceCode.substr(startPos,Cursor - 1 -startPos);
}

bool BDD::Lexer::IsLetter() {
    return (CurChar >= 'a' && CurChar <= 'z') || (CurChar >= 'A' && CurChar <= 'Z') || CurChar == '_';
}

bool BDD::Lexer::IsDigit() {
    return CurChar >= '0' && CurChar <= '9';
}

bool BDD::Lexer::IsLetterOrDigit() {
    return IsLetter() || IsDigit();
}

