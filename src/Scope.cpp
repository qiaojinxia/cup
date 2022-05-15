//
// Created by qiaojinxia on 2022/4/4.
//

#include "Scope.h"
#include "Common.h"

using namespace BDD;
void Scope::PushScope() {
    auto newScope = std::make_shared<ScopeItem>();
    newScope -> parent = CurScope;
    CurScope = newScope;
}

void Scope::PopScope() {
    CurScope = CurScope -> parent;
}

std::shared_ptr<Var> Scope::FindVar(std::string_view varName) {
    std::shared_ptr<ScopeItem> curScope = CurScope;
    auto varNode= curScope -> VarScope.find(varName);
    if (varNode != curScope ->VarScope.end()){
        return varNode -> second;
    }
    while(curScope -> parent){
        curScope = curScope->parent;
        varNode= curScope -> VarScope.find(varName);
        if (varNode != curScope ->VarScope.end()){
            return varNode -> second;
        }
    }
    return nullptr;
}

std::shared_ptr<Type> Scope::FindTag(std::string_view tagName) {
    std::shared_ptr<ScopeItem> curScope = CurScope;
    auto tagNode= curScope -> TypeScope.find(tagName);
    if (tagNode != curScope ->TypeScope.end()){
        return tagNode -> second;
    }
    while(curScope -> parent){
        curScope = curScope->parent;
        tagNode= curScope -> TypeScope.find(tagName);
        if (tagNode != curScope ->TypeScope.end()){
            return tagNode -> second;
        }
    }
    return nullptr;
}

void Scope::PushVar(std::shared_ptr<Var> var) {
    CurScope->VarScope[var->Name] = var;
}

void Scope::PushTag(std::string_view tagName, std::shared_ptr<Type> tag) {
    CurScope->TypeScope[tagName] = tag;
}

std::shared_ptr<Var> Scope::FindVarInCurrentScope(std::string_view varName) {
    std::shared_ptr<ScopeItem> curScope = CurScope;
    auto varNode= curScope -> VarScope.find(varName);
    if (varNode != curScope ->VarScope.end()){
        return varNode -> second;
    }
    return nullptr;
}

std::shared_ptr<Type> Scope::FindTagInCurrentScope(std::string_view tagName) {
    std::shared_ptr<ScopeItem> curScope = CurScope;
    auto tagNode= curScope -> TypeScope.find(tagName);
    if (tagNode != curScope ->TypeScope.end()){
        return tagNode -> second;
    }
    return nullptr;
}

bool Scope::CheckScopeDepthZero() {
    return false;
}

std::unordered_map<std::string, std::shared_ptr<ConstantNode>> Scope::GetConstantTable() {
    return ConstTable;
}


void Scope::PutToConstantTable(std::shared_ptr<ConstantNode> constantNode) {
    if (constantNode ->Name == ""){
        auto labelName = string_format("caomao_%d",countConstant ++);
        auto name = labelName.data();
        constantNode -> Name = name;
    }
    ConstTable[constantNode->Name] = constantNode;
}

void Scope::PushFuncSign(std::shared_ptr<FuncSign> funcSign) {
    FuncTable[funcSign->FuncName] = funcSign;
}

std::shared_ptr<FuncSign> Scope::GetFuncSign(std::string_view funcName) {
    if (FuncTable.find(funcName) == FuncTable.end()){
        return nullptr;
    }
    return FuncTable.find(funcName)->second;
}

