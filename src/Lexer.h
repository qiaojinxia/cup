//
// Created by a on 2022/3/8.
//

#ifndef BODDY_LEXER_H
#define BODDY_LEXER_H

#include <string_view>
#include <memory>
#include <unordered_map>



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
        Asterisk,
        Slash,
        Mod,
        Num,
        FloatNum,
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
        Float,
        Double,
        _Bool,
        Struct,
        Union,
        TypeDef,
        Return,
        Comma,
        Identifier,
        String,
        Assign,
        Amp,
        VerticalBar,
        Caret,
        Sal,
        Sar,
        SizeOf,
        Period,
        PointerTo,
        PPlus,
        MMinus,
        Break,
        Continue,
        SIGNED,
        UNSIGNED,
        Enum,
        Tilde,
        QuestionMark,
        Colon,
        ExclamationMark,
        Switch,
        Case,
        Default,
        Const,
    };
    class SourceLocation{
    public:
        int Line;
        int Col;
        int LineHead;
        int LineEnd;
        int FilePath;
        int Code;
    };
    class Token{
    public:
        TokenKind Kind;
        long Value;
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

        SourceLocation GetLocation();

    public:
        Lexer(const char *code){
            SourceCode = code;
        }
        void GetNextToken();
        void GetNextChar();
        void SkipToken(TokenKind  kind);
        void ExceptToken(TokenKind  kind);
        void ExceptedNextToken(TokenKind kind);
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


        bool IsHex();


    };


}
#endif //BODDY_LEXER_H
