//
// Created by qiaojinxia on 2022/3/20.
//

#include "Type.h"
#include <memory>


using namespace BDD;

std::shared_ptr<BuildInType> Type::VoidType = std::make_shared<BuildInType>(BuildInType::Kind::Void,8,8 ,"u64");
std::shared_ptr<BuildInType> Type::CharType = std::make_shared<BuildInType>(BuildInType::Kind::Char,1,1 ,"i8");
std::shared_ptr<BuildInType> Type::BoolType = std::make_shared<BuildInType>(BuildInType::Kind::Bool,1,1 ,"bool");
std::shared_ptr<BuildInType> Type::ShortType = std::make_shared<BuildInType>(BuildInType::Kind::Short ,2,2 ,"i16");
std::shared_ptr<BuildInType> Type::IntType = std::make_shared<BuildInType>(BuildInType::Kind::Int,4,4 ,"i32");
std::shared_ptr<BuildInType> Type::LongType = std::make_shared<BuildInType>(BuildInType::Kind::Long,8,8 ,"i64");
std::shared_ptr<BuildInType> Type::UCharType = std::make_shared<BuildInType>(BuildInType::Kind::UInt,1,1 ,"u8");
std::shared_ptr<BuildInType> Type::UShortType = std::make_shared<BuildInType>(BuildInType::Kind::UShort,2,2 ,"u8");
std::shared_ptr<BuildInType> Type::UIntType = std::make_shared<BuildInType>(BuildInType::Kind::UInt,4,4 ,"u32");
std::shared_ptr<BuildInType> Type::ULongType = std::make_shared<BuildInType>(BuildInType::Kind::ULong,8,8 ,"u64");
std::shared_ptr<BuildInType> Type::FloatType = std::make_shared<BuildInType>(BuildInType::Kind::Float ,4,4 ,"f32");
std::shared_ptr<BuildInType> Type::DoubleType = std::make_shared<BuildInType>(BuildInType::Kind::Double,8,8 ,"f64");
std::shared_ptr<PointerType> Type::Pointer = std::make_shared<PointerType>(ULongType);
std::shared_ptr<PointerType> Type::StringType = std::make_shared<PointerType>(CharType);


bool Type::IsIntegerNum() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsIntegerNum();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Int
        ||  build_in_type -> GetKind() == BuildInType::Kind::Char
        ||  build_in_type -> GetKind() == BuildInType::Kind::Short
        ||  build_in_type -> GetKind() == BuildInType::Kind::Long;
    }
    return false;
}

bool Type::IsUnsignedNum() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsUnsignedNum();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::UChar
                ||  build_in_type -> GetKind() == BuildInType::Kind::UShort
                    ||  build_in_type -> GetKind() == BuildInType::Kind::UInt
                        ||  build_in_type -> GetKind() == BuildInType::Kind::ULong;
    }
    return false;
}

bool Type::IsFunctionType() const {
    return TypeC == TypeClass::FuncType;
}

bool Type::IsStringType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsStringType();
    }
    if (TypeC == TypeClass::AryType){
        auto array_type = dynamic_cast<const ArrayType *>(this);
        return  array_type -> ElementType -> IsCharType();
    }
    return false;
}


bool Type::IsPointerType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsPointerType();
    }
    return TypeC == TypeClass::PtrType;
}

bool Type::IsArrayType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsArrayType();
    }
    return TypeC == TypeClass::AryType;
}

bool Type::IsFuncPointerType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsFuncPointerType();
    }
    if (TypeC == TypeClass::PtrType){
        auto ry = dynamic_cast<const PointerType *>(this);
        return ry->Base ->IsFunctionType() ;
    }
    return false;
}


bool Type::IsStructType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsStructType();
    }
    if (TypeC == TypeClass::RecordType){
        auto ry = dynamic_cast<const RecordType *>(this);
        return ry -> Kind  == RecordType::TagKind::Struct;
    }
    return false;
}

bool Type::IsUnionType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsUnionType();
    }
    if (TypeC == TypeClass::RecordType){
        auto ry = dynamic_cast<const RecordType *>(this);
        return ry -> Kind  == RecordType::TagKind::Union;
    }
    return false;
}

bool Type::IsFloatPointNum() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type->Base->IsFloatPointNum();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Float
                ||  build_in_type -> GetKind() == BuildInType::Kind::Double;
    }
    return false;
}

bool Type::IsIntType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsIntType();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Int;
    }
    return false;
}

bool Type::IsLongType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsLongType();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Long;
    }
    return false;
}

bool Type::IsCharType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsCharType();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Char;
    }
    return false;
}

bool Type::IsPtrCharType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type->Base->IsPtrCharType();
    }
    if (TypeC == TypeClass::PtrType){
        auto base_type = dynamic_cast<const PointerType *>(this);
        return  base_type ->Base ->IsCharType() ;
    }
    return false;
}



bool Type::IsShortType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsShortType();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Short;
    }
    return false;
}

bool Type::IsFloatType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsFloatType();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Float;
    }
    return false;
}

bool Type::IsDoubleType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsDoubleType();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::Double;
    }
    return false;
}

bool Type::IsUIntType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsUIntType();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::UInt;
    }
    return false;
}

bool Type::IsULongType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsULongType();
    }
    if (TypeC == TypeClass::BInType){
        auto build_in_type = dynamic_cast<const BuildInType *>(this);
        return  build_in_type -> GetKind() == BuildInType::Kind::ULong;
    }
    return false;
}

bool Type::IsBInType() const {
    if (TypeC == TypeClass::AliasType){
        auto alias_type = dynamic_cast<const AliasType *>(this);
        return alias_type ->Base->IsBInType();
    }
    return TypeC == TypeClass::BInType;
    return false;
}

bool Type::IsAliasType() const {
    return TypeC == TypeClass::AliasType;
}

bool Type::IsBoolType() const {
    return TypeC == TypeClass::BInType;
}

Type::TypeClass Type::GetTypeC() const {
    return TypeC;
}

bool Type::IsConstant() const {
    return TypeC == TypeClass::AliasType && constant;
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
    switch (Knd) {
        case Kind::Bool:
            return BoolType;
        case Kind::Char:
            return CharType;
        case Kind::UChar:
            return UCharType;
        case Kind::Short:
            return ShortType;
        case Kind::UShort:
            return UShortType;
        case Kind::Int:
            return IntType;
        case Kind::UInt:
            return UIntType;
        case Kind::Long:
            return LongType;
        case Kind::ULong:
            return ULongType;
        case Kind::Float:
            return FloatType;
        case Kind::Double:
            return DoubleType;
        case Kind::Void:
            return VoidType;
        default :
            assert(0);
    }
}

std::shared_ptr<BuildInType> BuildInType::GetBuildInType(BuildInType::Kind kind) {
    switch (kind) {
        case Void:
            return VoidType;
        case Char:
            return CharType;
        case Bool:
            return BoolType;
        case Short:
            return ShortType;
        case Int:
            return IntType;
        case Long:
            return LongType;
        case Float:
            return FloatType;
        case Double:
            return DoubleType;
        case Signed:
            assert(0);
        case UnSigned:
            assert(0);
        case UChar:
            return UCharType;
        case UShort:
            return UShortType;
        case UInt:
            return UIntType;
        case ULong:
            return ULongType;
        case UnDefine:
            assert(0);
    }
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
    auto recorderType = std::make_shared<RecordType>();
    recorderType ->fields = this ->fields;
    recorderType ->Kind = this ->Kind;
    return recorderType;
}

std::shared_ptr<Type> AliasType::GetBaseType() {
    return Base;
}

