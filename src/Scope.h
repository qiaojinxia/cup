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
    class FuncSign{
    public:
        FuncSign(std::shared_ptr<FunctionType> funcType) : FuncType(funcType) {};
        std::string_view  FuncName;
        std::shared_ptr<FunctionType> FuncType;
    };
    class Scope {
    public:
        std::unordered_map<std::string_view,std::shared_ptr<FuncSign>> FuncTable = {};
        int CountConstant{0};
    private:
        std::unordered_map<std::string,std::shared_ptr<ConstantNode>> ConstTable = {};
        std::unordered_map<std::string,std::unordered_map<std::string,std::shared_ptr<ConstantNode>>> StaticVarTable = {};
        class ScopeItem{
        public:
            std::unordered_map<std::string_view,std::shared_ptr<Var>> VarScope = {};
            std::unordered_map<std::string_view,std::shared_ptr<Type>> TypeScope = {};
            std::shared_ptr<ScopeItem> parent;
            std::string scopeTag ;
        };
    private:
        std::shared_ptr<ScopeItem> CurScope = {};
    public:
        void PutToConstantTable(std::shared_ptr<ConstantNode> constantNode);
        std::unordered_map<std::string,std::shared_ptr<ConstantNode>> GetConstantTable();

        void PushScope(std::string_view scopeTagName);
        void PopScope();

        void PushVar(std::shared_ptr<Var> var);
        std::shared_ptr<Var> FindVar(std::string_view varName);
        std::shared_ptr<Var> FindVarInCurrentScope(std::string_view varName);

        void PushFuncSign(std::shared_ptr<FuncSign>);
        std::shared_ptr<FuncSign> GetFuncSign(std::string_view funcName);

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

        std::unordered_map<std::string, std::shared_ptr<ConstantNode>>& GetStaticZeroVarTable();

        void PushStaticVar(std::string name, std::shared_ptr<ConstantNode> cstNode);
        std::string PushStaticVar(std::string_view name,std::shared_ptr<Type> type);

        std::unordered_map<std::string, std::shared_ptr<ConstantNode>>& GetStaticInitVarTable();

        std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<ConstantNode>>>
        GetStaticTable();

        std::shared_ptr<ConstantNode> GetStaticVar(std::string name);
    };

}



#endif //BODDY_SCOPE_H
