//
// Created by qiaojinxia on 2022/4/4.
//

#ifndef BODDY_SCOPE_H
#define BODDY_SCOPE_H
#include <stdio.h>
#include "AstNode.h"
#include "Type.h"
#include <unordered_map>

namespace BDD{
    class Scope;
    static std::shared_ptr<Scope> m_scopeInstance;
    class Scope {
    private:
        std::unordered_map<std::string,std::shared_ptr<ConstantNode>> ConstTable = {};
        int countConstant;
        class ScopeItem{
        public:
            std::unordered_map<std::string_view,std::shared_ptr<Var>> VarScope = {};
            std::unordered_map<std::string_view,std::shared_ptr<Type>> TypeScope = {};
            std::shared_ptr<ScopeItem> parent;
        };
    private:
        std::shared_ptr<ScopeItem> CurScope = {};
    public:
        void PutToConstantTable(std::shared_ptr<ConstantNode> constantNode);
        std::unordered_map<std::string,std::shared_ptr<ConstantNode>> GetConstantTable();

        void PushScope();
        void PopScope();

        void PushVar(std::shared_ptr<Var> var);
        std::shared_ptr<Var> FindVar(std::string_view varName);
        std::shared_ptr<Var> FindVarInCurrentScope(std::string_view varName);


        void PushTag(std::string_view tagName, std::shared_ptr<Type> tag);
        std::shared_ptr<Type> FindTag(std::string_view tagName);
        std::shared_ptr<Type> FindTagInCurrentScope(std::string_view tagName);

        bool CheckScopeDepthZero();

        static std::shared_ptr<Scope> GetInstance(){
            if (m_scopeInstance == nullptr){
                m_scopeInstance = std::shared_ptr<Scope>(new Scope);
            }
            return m_scopeInstance;
        }
    };

}


#endif //BODDY_SCOPE_H
