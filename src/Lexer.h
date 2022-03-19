//
// Created by a on 2022/3/8.
//

#ifndef BODDY_LEXER_H
#define BODDY_LEXER_H

#include <string_view>
#include <memory>

namespace BDD{
    enum class TokenKind{
        Equal,
        NotEqual,
        Greater,
        GreaterEqual,
        Lesser,
        LesserEqual,
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        Num,
        Eof,
        LParent,
        RParent,
        LBrace,
        RBrace,
        Semicolon,
        If,
        Else,
        While,
        Do,
        For,
        FunctionDefine,
        Return,
        Comma,
        Identifier,
        Assign,

    };
    class SourceLocation{
    public:
        int Line;
        int Col;
    };
    class Token{
    public:
        TokenKind Kind;
        int Value;
        std::string_view Content;
        SourceLocation Location;
    };
    class Lexer {
    private:
        char CurChar{' '};
        char Cursor{0};
        int Line{0};
        int LineHead{0};

        char PeekPointCurChar;
        int PeekPointCursor;
        int PeekPointLine;
        int PeekPointLineHead;
        std::shared_ptr<Token> PeekPointCurrentToken;

    public:
        std::shared_ptr<Token> CurrentToken;
        std::string_view SourceCode;
    public:
        Lexer(const char *code){
            SourceCode = code;
        }
        void GetNextToken();
        void GetNextChar();
        void ExceptToken(TokenKind  kind);
        void BeginPeekToken();
        void EndPeekToken();
    private:
        bool IsLetter();
        bool IsDigit();
        bool IsLetterOrDigit();
        char PeekChar(int n);
        const char* GetTokenName(TokenKind kind);

    };

}
#endif //BODDY_LEXER_H
