//
// Created by qiaojinxia on 2022/5/18.
//
#include <string>
#include <unordered_map>
#ifndef CUP_PREPROCESSING_H
#define CUP_PREPROCESSING_H

namespace BDD{
    using DirectiveMap = std::unordered_map<std::string, int>;
//    static const DirectiveMap directiveMap {
//            {"if", Token::PP_IF},
//            {"ifdef", Token::PP_IFDEF},
//            {"ifndef", Token::PP_IFNDEF},
//            {"elif", Token::PP_ELIF},
//            {"else", Token::PP_ELSE},
//            {"endif", Token::PP_ENDIF},
//            {"include", Token::PP_INCLUDE},
//            // Non-standard GNU extension
//            {"include_next", Token::PP_INCLUDE},
//            {"define", Token::PP_DEFINE},
//            {"undef", Token::PP_UNDEF},
//            {"line", Token::PP_LINE},
//            {"error", Token::PP_ERROR},
//            {"pragma", Token::PP_PRAGMA}
//    };
    class PreProcessing {
    private:
        std::string_view source;
    public:
        PreProcessing(const char * s) :source(s){};
        std::string Scan();
    };
}



#endif //CUP_PREPROCESSING_H
