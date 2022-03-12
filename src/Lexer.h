//
// Created by a on 2022/3/8.
//

#ifndef BODDY_LEXER_H
#define BODDY_LEXER_H

#include <string_view>
#include <memory>

namespace BDD{
    enum class TokenKind{
        Add,
        Sub,
        Mul,
        Div,
        Num,
        Eof,
        LParen,
        RParen,
    };
    class Token{
    public:
        TokenKind Kind;
        int Value;
        std::string_view Content;
    };
    class Lexer {
    private:
        std::string_view SourceCode;
        char CurChar{' '};
        char Cursor{0};
    public:
        std::shared_ptr<Token> CurrentToken;
    public:
        Lexer(const char *code){
            SourceCode = code;
        }
        void GetNextToken();
        void GetNextChar();

    };
}
#endif //BODDY_LEXER_H
