//
// Created by qiaojinxia on 2022/4/4.
//

#include "Scope.h"

using namespace BDD;
void Scope::EnterScope() {
    scopes.push_back(std::make_shared<ScopeItem>());
}

void Scope::LeaveScope() {
    scopes.pop_back();
}

std::shared_ptr<Var> Scope::FindVar(std::string_view varName) {
    for (auto &scope: scopes) {
        auto var = scope -> VarScope.find(varName);
        if (var != scope ->VarScope.end()){
            return var->second;
        }
    }
    return nullptr;
}

std::shared_ptr<Type> Scope::FindTag(std::string_view typeName) {
    for (auto &scope: scopes) {
        auto type = scope -> TypeScope.find(typeName);
        if (type != scope -> TypeScope.end()){
            return type -> second;
        }
    }
    return nullptr;
}

void Scope::PushVar(std::shared_ptr<Var> var) {
    auto curScope = scopes.back();
    curScope->VarScope[var->Name] = var;
}

void Scope::PushTag(std::string_view tagName, std::shared_ptr<Type> tag) {
    auto curScope = scopes.back();
    curScope->TypeScope[tagName] = tag;
}

std::shared_ptr<Var> Scope::FindVarInCurrentScope(std::string_view name) {
    auto curScope = scopes.back();
    auto var = curScope ->VarScope.find(name);
    if (var !=  curScope ->VarScope.end()){
            return var->second;
    }
    return nullptr;
}

std::shared_ptr<Type> Scope::FindTagInCurrentScope(std::string_view tag) {
    auto curScope = scopes.back();
    auto type = curScope ->TypeScope.find(tag);
    if (type !=  curScope -> TypeScope.end()){
        return type->second;
    }
    return nullptr;
}


