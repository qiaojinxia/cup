//
// Created by qiaojinxia on 2022/5/18.
//
#include <string>
#ifndef CUP_PREPROCESSING_H
#define CUP_PREPROCESSING_H

namespace BDD{
    class PreProcessing {
    private:
        std::string_view source;
    public:
        PreProcessing(const char * s) :source(s){};
        std::string Scan();
    };
}



#endif //CUP_PREPROCESSING_H
