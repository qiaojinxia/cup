//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cctype>
#include "Lexer.h"
#include "Diag.h"
#include <string>
#include <cmath>

using namespace BDD;
void BDD::Lexer::GetNextChar() {
    if (Cursor == SourceCode.size()){
        CurChar = '\0';
        return;
    }
    CurChar = SourceCode[Cursor++];
}

void BDD::Lexer::GetNextToken() {
    SkipWhiteSpace();
    int value = 0;
    TokenKind kind;
    int startPos = Cursor -1;
    if (CurChar == '\0'){
        kind = TokenKind::Eof;
    }else if(CurChar == '+'){
        switch (PeekChar(1)) {
            case '+':
                GetNextChar();
                kind = TokenKind::PPlus;
                break;
            default:
                kind = TokenKind::Plus;
        }
        GetNextChar();
    }else if(CurChar == '-'){
        switch (PeekChar(1)) {
            case '-':
                GetNextChar();
                kind = TokenKind::MMinus;
                break;
            case '>':
                GetNextChar();
                kind = TokenKind::PointerTo;
                break;
            default:
                kind = TokenKind::Minus;
        }
        GetNextChar();
    }else if (CurChar == ','){
        kind = TokenKind::Comma;
        GetNextChar();
    }else if(CurChar == '*'){
        kind = TokenKind::Asterisk;
        GetNextChar();
    }else if(CurChar == '/'){
        kind = TokenKind::Slash;
        GetNextChar();
    }else if(CurChar == '%'){
        kind = TokenKind::Mod;
        GetNextChar();
    }else if(CurChar == '('){
        kind = TokenKind::LParent;
        GetNextChar();
    }else if(CurChar == '&'){
        kind = TokenKind::Amp;
        GetNextChar();
    }else if(CurChar == ')'){
        kind = TokenKind::RParent;
        GetNextChar();
    }else if(CurChar == '['){
        kind = TokenKind::LBracket;
        GetNextChar();
    }else if(CurChar == ']'){
        kind = TokenKind::RBracket;
        GetNextChar();
    }else if(CurChar == '{'){
        kind = TokenKind::LBrace;
        GetNextChar();
    }else if(CurChar == '}'){
        kind = TokenKind::RBrace;
        GetNextChar();
    }else if(CurChar == ';'){
        kind = TokenKind::Semicolon;
        GetNextChar();
    }else if(CurChar == '.'){
        kind = TokenKind::Period;
        GetNextChar();
    }else if(CurChar == '|'){
        kind = TokenKind::VerticalBar;
        GetNextChar();
    }else if(CurChar == '^'){
        kind = TokenKind::Caret;
        GetNextChar();
    }else if(CurChar == '='){
        if (PeekChar(1)=='='){
            GetNextChar();
            kind = TokenKind::Equal;
        }else{
            kind = TokenKind::Assign;
        }
        GetNextChar();
    }else if (CurChar == '!'){
        if (PeekChar(1) == '='){
            GetNextChar();
            kind = TokenKind::NotEqual;
        }else{
            DiagLoc(SourceCode,CurrentToken->Location,"token '%c' is illegal",CurChar);
        }
        GetNextChar();
    }else if(isdigit(CurChar)){
        kind = TokenKind::Num;
        value = 0;
        int n = 0;
        do {
            if (kind == TokenKind::FloatNum){
                n+= 1;
            }
            value = value * 10 + CurChar - '0';
            GetNextChar();
            if (CurChar == '.'){
                kind = TokenKind::FloatNum;
                GetNextChar();
            }
        }while (isdigit(CurChar));
        if (kind == TokenKind::FloatNum){
            float a = value / pow(10,n);
            int *m = (int*)&a;
            value = *m;
        }
    }else if (CurChar == '>'){
        switch (PeekChar(1)) {
            case '=':
                GetNextChar();
                kind = TokenKind::GreaterEqual;
                break;
            case '>':
                GetNextChar();
                kind = TokenKind::Sar;
                break;
            default:
                kind = TokenKind::Greater;
        }
        GetNextChar();
    }else if (CurChar == '<'){
        switch (PeekChar(1)) {
            case '=':
                GetNextChar();
                kind = TokenKind::LesserEqual;
                break;
            case '<':
                GetNextChar();
                kind = TokenKind::Sal;
                break;
            default:
                kind = TokenKind::Lesser;
        }
        GetNextChar();
    }else{
        if (IsLetter()){
            GetNextChar();
            while(IsLetterOrDigit()){
                GetNextChar();
            }
            kind = TokenKind::Identifier;
            std::string_view content = SourceCode.substr(startPos,Cursor - 1 - startPos);
            if (content  == "if"){
                kind = TokenKind::If;
            }else if (content == "else"){
                kind = TokenKind::Else;
            }else if (content == "while"){
                kind = TokenKind::While;
            }else if (content == "do"){
                kind = TokenKind::Do;
            }else if (content == "for"){
                kind = TokenKind::For;
            }else if (content == "int"){
                kind = TokenKind::Int;
            }else if (content == "return"){
                kind = TokenKind::Return;
            }else if (content == "sizeof"){
                kind = TokenKind::SizeOf;
            }else if(content == "char"){
                kind = TokenKind::Char;
            }else if(content == "short"){
                kind = TokenKind::Short;
            }else if (content == "long"){
                kind = TokenKind::Long;
            }else if(content == "struct"){
                kind = TokenKind::Struct;
            }else if(content == "union"){
                kind = TokenKind::Union;
            }else if(content == "break"){
                kind = TokenKind::Break;
            }else if(content == "continue"){
                kind = TokenKind::Continue;
            }else if(content == "float"){
                kind = TokenKind::Float;
            }else if(content == "double"){
                kind = TokenKind::Double;
            }
        }else{
            DiagLoc(SourceCode,CurrentToken->Location,"token '%c' is illegal",CurChar);
        }
    }
    CurrentToken = std::make_shared<Token>();
    CurrentToken->Kind = kind;
    CurrentToken->Value = value;
    CurrentToken -> Location = GetLocation();
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

void Lexer::ExceptToken(TokenKind kind) {
    if (CurrentToken->Kind == kind){
        GetNextToken();
    }else{
        DiagLoc(SourceCode,CurrentToken->Location,"'%s' excepted",GetTokenName(kind));
    }
}

const char *Lexer::GetTokenName(TokenKind kind) {
    switch (kind) {
        case TokenKind::Plus:
            return "+";
        case TokenKind::Minus:
            return "-";
        case TokenKind::Asterisk:
            return "*";
        case TokenKind::Slash:
            return "/";
        case TokenKind::Semicolon:
            return ";";
        case TokenKind::LParent:
            return "(";
        case TokenKind::RParent:
            return ")";
        case TokenKind::LBracket:
            return "[";
        case TokenKind::RBracket:
            return "]";
        case TokenKind::Assign:
            return "=";
        case TokenKind::Eof:
            return "eof ";
        case TokenKind::SizeOf:
            return "sizeof";
        case TokenKind::Comma:
            return ",";
        case TokenKind::Continue:
            return "continue";
        default:
            assert(0);
    }
    return "";
}

char Lexer::PeekChar(int n) {
    assert(n >0);
    if (Cursor - 1 + n < SourceCode.size()){
        return SourceCode[Cursor - 1 + n];
    }
    return '\0';
}

void Lexer::EndPeekToken() {
    CurChar = PeekPointCurChar;
    Cursor = PeekPointCursor;
    Line = PeekPointLine;
    LineHead = PeekPointLineHead;
    CurrentToken = PeekPointCurrentToken;
}

void Lexer::BeginPeekToken() {
    PeekPointCurChar = CurChar;
    PeekPointCursor = Cursor;
    PeekPointLine = Line;
    PeekPointLineHead = LineHead;
    PeekPointCurrentToken = CurrentToken;
}

void Lexer::SkipWhiteSpace() {
    while (isspace(CurChar) || (CurChar == '/' && PeekChar(1) ==   '/')
    || (CurChar == '/' && PeekChar(1) == '*')){
        if(CurChar == '/'){
            SkipComment();
            continue;
        }else if (CurChar== '\n'){
            Line ++;
            LineHead = Cursor;
        }
        GetNextChar();
    }
}

void Lexer::SkipComment() {
    if(CurChar == '/' && PeekChar(1) == '/'){
        while (CurChar != '\n'){
            LineHead = Cursor -1;
            GetNextChar();
        }
    }else{
        auto pos = SourceCode.find("*/",Cursor + 1);
        if (pos == std::string_view::npos){
            printf("unclosed \"*/\"");
            assert(0);
        }else{
            CurChar= PeekChar((pos + 2) - (Cursor - 1));
            Cursor = pos + 3;
        }
    }
}

SourceLocation Lexer::GetLocation(){
    SourceLocation Location;
    Location.Line = Line;
    Location.Col = Cursor - 1 -LineHead;
    Location.LineHead = LineHead;
    int offset = 0;
    int cur = Cursor;
    while(SourceCode[cur + offset] != '\n'){
       offset += 1;
    }
    Location.LineEnd = offset;
    return Location;
}

void Lexer::SkipToken(TokenKind kind) {
    GetNextToken();
    if (CurrentToken->Kind == kind){
        GetNextToken();
    }
}


