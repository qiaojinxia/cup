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
        Plus,
        Minus,
        Start,
        Slash,
        Mod,
        Num,
        Eof,
        LParent,
        RParent,
        LBrace,
        RBrace,
        LBracket,
        RBracket,
        Semicolon,
        If,
        Else,
        While,
        Do,
        For,
        Int,
        Char,
        Short,
        Long,
        Struct,
        Union,
        Return,
        Comma,
        Identifier,
        Assign,
        Amp,
        SizeOf,
        Period,

    };
    class SourceLocation{
    public:
        int Line;
        int Col;
        int LineHead;
        int FilePath;
        int Code;
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
        int Cursor{0};
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
        void SkipToken(TokenKind  kind);
        void ExceptToken(TokenKind  kind);
        void BeginPeekToken();
        void EndPeekToken();
    private:
        bool IsLetter();
        bool IsDigit();
        bool IsLetterOrDigit();
        char PeekChar(int n);
        const char* GetTokenName(TokenKind kind);

        void SkipWhiteSpace();
        void SkipComment();


        SourceLocation GetLocation();
    };

}
#endif //BODDY_LEXER_H
