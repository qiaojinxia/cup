//
// Created by qiaojinxia on 2022/3/20.
//

#include "Type.h"
#include <memory>


using namespace BDD;

std::shared_ptr<BuildInType> Type::IntType = std::make_shared<BuildInType>(BuildInType::Kind::Int,4,4 ,"i32");
std::shared_ptr<BuildInType> Type::CharType = std::make_shared<BuildInType>(BuildInType::Kind::Char,1,1 ,"i8");
std::shared_ptr<BuildInType> Type::ShortType = std::make_shared<BuildInType>(BuildInType::Kind::Short ,2,2 ,"i16");
std::shared_ptr<BuildInType> Type::LongType = std::make_shared<BuildInType>(BuildInType::Kind::Long,8,8 ,"i64");

std::shared_ptr<BuildInType> Type::FloatType = std::make_shared<BuildInType>(BuildInType::Kind::Float ,4,4 ,"f32");
std::shared_ptr<BuildInType> Type::DoubleType = std::make_shared<BuildInType>(BuildInType::Kind::Double,8,8 ,"f64");
std::shared_ptr<PointerType> Type::Pointer = std::make_shared<PointerType>(LongType);


bool Type::IsIntegerNum() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Int
        ||  build_in_type -> GetKind() == BuildInType::Kind::Char
        ||  build_in_type -> GetKind() == BuildInType::Kind::Short
        ||  build_in_type -> GetKind() == BuildInType::Kind::Long;
    }
    return false;
}

bool Type::IsFunctionType() const {
    return TypeC == TypeClass::FuncType;
}

bool Type::IsPointerType() const {
    return TypeC == TypeClass::PtrType;
}

bool Type::IsArrayType() const {
    return TypeC == TypeClass::AryType;
}

bool Type::IsStructType() const {
    if (TypeC == TypeClass::RecordType){
        auto ry = dynamic_cast<const RecordType *>(this);
        return ry -> Kind  == RecordType::TagKind::Struct;
    }
    return false;
}

bool Type::IsUnionType() const {
    if (TypeC == TypeClass::RecordType){
        auto ry = dynamic_cast<const RecordType *>(this);
        return ry -> Kind  == RecordType::TagKind::Union;
    }
    return false;
}

bool Type::IsFloatNum() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Float
                ||  build_in_type -> GetKind() == BuildInType::Kind::Double;
    }
    return false;
}

bool Type::IsIntType() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Int;
    }
    return false;
}

bool Type::IsLongType() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Long;
    }
    return false;
}

bool Type::IsCharType() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Char;
    }
    return false;
}

bool Type::IsShortType() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Short;
    }
    return false;
}

bool Type::IsFloatType() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Float;
    }
    return false;
}

bool Type::IsDoubleType() const {
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Double;
    }
    return false;
}



std::shared_ptr<Field> RecordType::GetField(std::string_view fieldName) {
    for(auto &field:fields){
        if (field->token->Content == fieldName){
            return field;
        }
    }
    return nullptr;
}


std::shared_ptr<Type> BuildInType::GetBaseType() {
    return nullptr;
}

std::shared_ptr<Type> PointerType::GetBaseType() {
    return Base;
}

std::shared_ptr<Type> ArrayType::GetBaseType() {
    return ElementType;
}

std::shared_ptr<Type> FunctionType::GetBaseType() {
    return ReturnType;
}

std::shared_ptr<Type> RecordType::GetBaseType() {
    return nullptr;
}
