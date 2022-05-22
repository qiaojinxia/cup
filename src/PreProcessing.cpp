//
// Created by qiaojinxia on 2022/5/18.
//

#include "PreProcessing.h"
#include "Common.h"
#include <string>
#include <vector>

using namespace BDD;
std::string BDD::PreProcessing::Scan() {
    int  i =  0;
    int line = 1;
    std::string originCode = std::string(source);
    std::string newCode = "";
    std::vector<std::vector<std::string>> rule ;
    while (originCode[i] != '\0') {
        int begin = i;
        while (originCode[i] != '\n') {
            i++;
        }
        auto CurLine = originCode.substr(begin, i - begin);
        std::string codeLine = std::string(CurLine);
        if (is_contains_str(codeLine.data(), "#define")) {
            auto _tmp1 = originCode.substr(0, begin );
            auto _tmp2 = originCode.substr(i - begin +1, originCode.size());
            originCode = _tmp1 + _tmp2;
            i = begin;
            auto words = split_str(codeLine,' ');
            rule.push_back(words);
        }else{
            i++;
        }
    }
    for(auto & j : rule)
        string_replace(originCode,j[2],j[1]);
    return originCode;
}