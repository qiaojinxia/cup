//
// Created by a on 2022/3/8.
//

#include <cstdio>
#include <cctype>
#include "Lexer.h"
#include "Diag.h"
#include <string>
#include <cmath>
#include "Common.h"

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
    unsigned long value = 0;
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
            case '=':
                GetNextChar();
                kind = TokenKind::PlusAssign;
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
            case '=':
                GetNextChar();
                kind = TokenKind::MinusAssign;
                break;
            case '>':
                GetNextChar();
                kind = TokenKind::PointerTo;
                break;
            default:
                kind = TokenKind::Minus;
        }
        GetNextChar();
//        // - xxx is num
//        if (kind == TokenKind::Minus){
//            if (IsDigit()){
//                GetNextToken();
//            }
//            auto curTk = CurrentToken;
//            kind = curTk->Kind;
//            value = curTk->Value * -1;
//        }
    }else if (CurChar == ','){
        kind = TokenKind::Comma;
        GetNextChar();
    }else if (CurChar == '\''){
        kind = TokenKind::CharNum;
        GetNextChar();
        if (CurChar == '\\'){
            GetNextChar();
            switch (CurChar) {
                case 'a':
                    CurChar = 7;
                    break;
                case 'b':
                    CurChar = 8;
                    break;
                case 'f':
                    CurChar = 12;
                    break;
                case 'n':
                    CurChar = 10;
                    break;
                case 'r':
                    CurChar = 13;
                    break;
                case 't':
                    CurChar = 9;
                    break;
                case 'v':
                    CurChar = 11;
                    break;
                case '\\':
                    CurChar = 92;
                    break;
                case '\?':
                    CurChar = 63;
                    break;
                case '\'':
                    CurChar = 39;
                    break;
                case '\"':
                    CurChar = 34;
                    break;
                case '\0':
                    CurChar = 0;
                    break;
                default:
                    break;
            }
        }
        value = CurChar;
        GetNextChar();
        if (CurChar != '\'')
            printf("excepted ' ");
        GetNextChar();
    }else if(CurChar == '*'){
        GetNextChar();
        switch (CurChar) {
            case '=':
                kind = TokenKind::AsteriskAssign;
                GetNextChar();
                break;
            default:
                kind = TokenKind::Asterisk;
                break;
        }
    }else if(CurChar == '/'){
        GetNextChar();
        switch (CurChar) {
            case '=':
                kind = TokenKind::SlashAssign;
                GetNextChar();
                break;
            default:
                kind = TokenKind::Slash;
                break;
        }
    }else if(CurChar == '%'){
        GetNextChar();
        if (CurChar == '='){
            GetNextChar();
            kind = TokenKind::ModAssign;
        }else{
            kind = TokenKind::Mod;
        }
    }else if(CurChar == '('){
        kind = TokenKind::LParent;
        GetNextChar();
    }else if(CurChar == '&'){
        GetNextChar();
        if (CurChar =='&'){
            GetNextChar();
            kind = TokenKind::And;
        }else if (CurChar =='='){
            GetNextChar();
            kind = TokenKind::AmpAssign;
        }else{
            kind = TokenKind::Amp;
        }
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
        GetNextChar();
        if (CurChar =='|'){
            GetNextChar();
            kind = TokenKind::Or;
        }else if (CurChar =='='){
            GetNextChar();
            kind = TokenKind::VerticalBarAssign;
        }else{
            kind = TokenKind::VerticalBar;
        }
    }else if(CurChar == '^'){
        GetNextChar();
        if (CurChar =='='){
            GetNextChar();
            kind = TokenKind::CaretAssign;
        }else{
            kind = TokenKind::Caret;
        }
    }else if(CurChar == '~'){
        kind = TokenKind::Tilde;
        GetNextChar();
    }else if(CurChar == ':'){
        kind = TokenKind::Colon;
        GetNextChar();
    }else if(CurChar == '?'){
        kind = TokenKind::QuestionMark;
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
            kind = TokenKind::ExclamationMark;
        }
        GetNextChar();
    }else if(isdigit(CurChar)){
        kind = TokenKind::Num;
        value = 0;
        std::string sValue;
        int n = 0;
        int  begin ;
        if (CurChar == '0' && PeekChar(1) =='x'){
            GetNextChar();
            int count = -1;
            begin = Cursor;
            do {
                GetNextChar();
                count ++;
            } while (IsHex());
            value = hexToDec(SourceCode.substr(begin, count), count);
        }else if(CurChar == '0' && PeekChar(1) =='b'){
            GetNextChar();
            int count = -1;
            begin = Cursor;
            do {
                GetNextChar();
                count ++;
            } while (IsHex());
            value = binToDec(SourceCode.substr(begin, count), count);
        }else{
            NumWhile:
            do {
                if (kind == TokenKind::DoubleNum){
                    n+= 1;
                }
                value = value * 10 + CurChar - '0';
                sValue += CurChar;
                GetNextChar();
                if (CurChar == '.'){
                    sValue += CurChar;
                    kind = TokenKind::DoubleNum;
                    GetNextChar();
                }
            }while (isdigit(CurChar));
            if(kind == TokenKind::DoubleNum){
                double a = atof(sValue.c_str());
                unsigned long *m = (unsigned long *)&a;
                value = *m;
            }
            Begin:
            if (CurChar == 'L' || CurChar == 'l'){
                GetNextChar();
                kind = TokenKind::Long;
            }else if(CurChar == 'F' || CurChar == 'f'){
                float a = atof(sValue.c_str());
                unsigned long *m = (unsigned long *)&a;
                value = *m;
                GetNextChar();
                kind = TokenKind::FloatNum;
            }else if(CurChar == '.'){
                GetNextChar();
                kind = TokenKind::DoubleNum;
                goto Begin;
            }else if(CurChar == 'e'){
                GetNextChar();
                goto NumWhile;
            }
        }
    }else if (CurChar == '>'){
        GetNextChar();
        switch (CurChar) {
            case '=':
                GetNextChar();
                kind = TokenKind::GreaterEqual;
                break;
            case '>':
                GetNextChar();
                if (CurChar =='='){
                    GetNextChar();
                    kind = TokenKind::SarAssign;
                }else{
                    kind = TokenKind::Sar;
                }
                break;
            default:
                kind = TokenKind::Greater;
        }
    }else if (CurChar == '<'){
        GetNextChar();
        switch (CurChar) {
            case '=':
                GetNextChar();
                kind = TokenKind::LesserEqual;
                break;
            case '<':
                GetNextChar();
                if (CurChar =='='){
                    GetNextChar();
                    kind = TokenKind::SalAssign;
                }else{
                    kind = TokenKind::Sal;
                }
                break;
            default:
                kind = TokenKind::Lesser;
        }
    }else if(CurChar == '"'){
        GetNextChar();
        while(CurChar !='"'){
            GetNextChar();
        }
        GetNextChar();
        kind = TokenKind::String;

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
            }else if(content == "signed"){
                kind = TokenKind::SIGNED;
            }else if(content == "unsigned"){
                kind = TokenKind::UNSIGNED;
            }else if(content == "typedef"){
                kind = TokenKind::TypeDef;
            }else if(content == "_Bool"){
                kind = TokenKind::_Bool;
            }else if(content == "enum"){
                kind = TokenKind::Enum;
            }else if(content == "switch"){
                kind = TokenKind::Switch;
            }else if(content == "case"){
                kind = TokenKind::Case;
            }else if(content == "default"){
                kind = TokenKind::Default;
            }else if(content == "const"){
                kind = TokenKind::Const;
            }else if(content == "extern"){
                kind = TokenKind::Extern;
            }else if(content == "static"){
                kind = TokenKind::Static;
            }else if(content == "void"){
                kind = TokenKind::Void;
            }
        }else{
            DiagLoc(SourceCode,CurrentToken->Location,"token '%c' is illegal",CurChar);
        }
    }
    CurrentToken = std::make_shared<Token>();
    CurrentToken->Kind = kind;
    CurrentToken->Value = value;
    StartPos = startPos;
    CurrentToken -> Location = GetLocation();
    CurrentToken->Content = SourceCode.substr(startPos,Cursor - 1 -startPos);
}

bool BDD::Lexer::IsLetter() {
    return (CurChar >= 'a' && CurChar <= 'z') || (CurChar >= 'A' && CurChar <= 'Z') || CurChar == '_';
}

bool BDD::Lexer::IsDigit() {
    return CurChar >= '0' && CurChar <= '9';
}


bool BDD::Lexer::IsHex() {
    return (CurChar >= '0' && CurChar <= '9') || (CurChar >= 'A' && CurChar <= 'F') || (CurChar >= 'a' && CurChar <= 'f');
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
        case TokenKind::Colon:
            return ":";
        case TokenKind::Continue:
            return "continue";
        case TokenKind::Enum:
            return "enum";
        case TokenKind::RBrace:
            return "{";
        case TokenKind::LBrace:
            return "}";
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
            LineHead = Cursor + 1;
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
    Location.ColBegin = StartPos - 1 -LineHead;
    Location.LineHead = LineHead;
    int offset = 0;
    int cur = Cursor;
    while(cur + offset <SourceCode.size() && SourceCode[cur + offset] != '\n' ){
       offset += 1;
    }
    Location.LineEnd = offset;
    return Location;
}

void Lexer::SkipToken(TokenKind kind) {
    GetNextToken();
    if (CurrentToken->Kind == kind){
        GetNextToken();
        return;
    }
}

void Lexer::ExceptedNextToken(TokenKind kind) {
    GetNextToken();
    if (CurrentToken->Kind != kind){
        DiagLoc(SourceCode,CurrentToken->Location,"'%s' excepted",GetTokenName(kind));
    }
}




