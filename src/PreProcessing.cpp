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
    int offset =-1;

    while (source[i] != '\0') {
        int begin = i;
        while(source[i] != '\n'){
            i++;
        }
        auto CurLine = source.substr(begin,i-begin);
        std::string codeLine = std::string (CurLine);
        if(is_contains_str(codeLine.data(),"#宏")){
            if (offset==-1)
                offset = begin;
            auto words = split_str(codeLine,' ');
            if (words[1] == "吧啦啦能量沙罗沙罗小魔仙全身变"){
                rule.push_back(words);
            }
        }else if (offset!=-1){
            auto _tmp1 = originCode.substr(0,offset);
            auto _tmp2 = originCode.substr(begin,originCode.size());
            originCode = _tmp1 + _tmp2;
            offset = -1;
        }
        i++;
    }

    for (int j = 0; j < rule.size(); j++) {
        string_replace(originCode,rule[j][3],rule[j][2]);
    }

    return originCode;
}

