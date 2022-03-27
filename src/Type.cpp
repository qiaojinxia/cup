//
// Created by qiaojinxia on 2022/3/20.
//

#include "Type.h"
#include <memory>
#include "Type.h"

using namespace BDD;

std::shared_ptr<BuildInType> Type::IntType = std::make_shared<BuildInType>(BuildInType::Kind::Int,8,8 );

bool Type::IsIntegerType() const {
    if (TypeC == TypeClass::BuildInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Int;
    }
    return false;
}

bool Type::IsFunctionType() const {
    return TypeC == TypeClass::FunctionType;
}

bool Type::IsPointerType() const {
    return TypeC == TypeClass::PointerType;
}



