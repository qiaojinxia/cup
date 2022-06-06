//
// Created by qiaojinxia on 2022/4/4.
//

#include "Scope.h"
#include "Common.h"

using namespace BDD;
void Scope::PushScope(std::string_view scopeTagName) {
    if (scopeTagName == ""){
        scopeTagName = CurScope->scopeTag;
    }
    auto newScope = std::make_shared<ScopeItem>();
    newScope -> parent = CurScope;
    newScope ->scopeTag = std::string(scopeTagName).data();
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

std::shared_ptr<ConstantNode>  Scope::GetStaticVar(std::string name){
    auto stZVar = GetStaticZeroVarTable().find(name);
    if (stZVar != GetStaticZeroVarTable().end()){
        return stZVar->second;
    }
    return nullptr;
}

void Scope::PushStaticVar(std::string name,std::shared_ptr<ConstantNode> cstNode){
    if (cstNode ->Value == 0 && !cstNode ->HasSetValue()){
        return;
    }
    auto stZVar = GetStaticZeroVarTable().find(name);
    if (stZVar != GetStaticZeroVarTable().end()){
        GetStaticZeroVarTable().erase(name);
        ConstTable.erase(cstNode->Name);
        cstNode ->Name = name;
        GetStaticInitVarTable()[name] = cstNode;
    }
}


std::unordered_map<std::string,std::unordered_map<std::string,std::shared_ptr<ConstantNode>>> Scope::GetStaticTable() {
    return StaticVarTable;
}

std::unordered_map<std::string,std::shared_ptr<ConstantNode>>& Scope::GetStaticZeroVarTable() {
    auto sTableZero = StaticVarTable.find(".bss");
    if (sTableZero == StaticVarTable.end()){
        StaticVarTable[".bss"] = std::unordered_map<std::string,std::shared_ptr<ConstantNode>>();
    }
    return StaticVarTable[".bss"];
}

std::unordered_map<std::string,std::shared_ptr<ConstantNode>>& Scope::GetStaticInitVarTable() {
    auto sTableInit = StaticVarTable.find(".data");
    if (sTableInit == StaticVarTable.end()){
        StaticVarTable[".data"] = std::unordered_map<std::string,std::shared_ptr<ConstantNode>>();
    }
    return StaticVarTable[".data"];
}


void Scope::PutToConstantTable(std::shared_ptr<ConstantNode> constantNode) {
    if (constantNode ->Name == ""){
        auto labelName = string_format("const.caomao.%d",CountConstant ++);
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

std::string Scope::PushStaticVar(std::string_view name,std::shared_ptr<Type> type) {
    auto cstNode = std::make_shared<ConstantNode>(nullptr);
    cstNode->isChange = true;
    cstNode ->Value = 0;
    cstNode ->Type = type;
    std::string varName(name);
    std::string curVarScopeTag = string_format("%s.%s",CurScope->scopeTag.data(),varName.data());
    cstNode ->Name = curVarScopeTag;
    GetStaticZeroVarTable()[curVarScopeTag] = cstNode;
    return curVarScopeTag;
}

