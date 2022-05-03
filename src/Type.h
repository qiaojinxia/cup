//
// Created by qiaojinxia on 2022/3/20.
//

#ifndef BODDY_TYPE_H
#define BODDY_TYPE_H
#include <memory>
#include "Lexer.h"
#include <list>


namespace BDD{
    class BuildInType;
    class FunctionType;
    class PointerType;
    class ArrayType;
    class RecordType;
    class AliasType;
    class Type {
    public:
        static std::shared_ptr<BuildInType> VoidType;
        static std::shared_ptr<BuildInType> IntType;
        static std::shared_ptr<BuildInType> UIntType;
        static std::shared_ptr<PointerType> Pointer;
        static std::shared_ptr<BuildInType> CharType;
        static std::shared_ptr<BuildInType> UCharType;
        static std::shared_ptr<BuildInType> ShortType;
        static std::shared_ptr<BuildInType> UShortType;
        static std::shared_ptr<BuildInType> LongType;
        static std::shared_ptr<BuildInType> ULongType;
        static std::shared_ptr<BuildInType> FloatType;
        static std::shared_ptr<BuildInType> DoubleType;
        static std::shared_ptr<PointerType> StringType;
        enum class TypeClass{
            BInType,
            PtrType,
            FuncType,
            AryType,
            RecordType,
            AliasType,
        };
        int Size;
        int Align;
        const char * Alias;
    private:
        TypeClass TypeC;
    public:
        virtual ~Type(){};
        Type(TypeClass tc,int size,int align, const char * alias ) : TypeC(tc) , Size(size),Align(align),Alias(alias){};
        bool IsIntegerNum() const;
        bool IsFloatNum() const;
        virtual std::shared_ptr<Type> GetBaseType(){return nullptr;};

        bool IsFunctionType() const;
        bool IsPointerType() const;
        bool IsArrayType() const;
        bool IsIntType() const;
        bool IsCharType() const;
        bool IsShortType() const;
        bool IsFloatType() const;
        bool IsDoubleType() const;
        bool IsStructType() const;
        bool IsUnionType() const;
        bool IsStringType() const;
        bool IsLongType() const;

        bool IsUnsignedNum() const;

        bool IsUIntType() const;

        bool IsULongType() const;


        bool IsAliasType() const;
    };


    class BuildInType: public Type{
    public:
        enum  Kind{
            Void = 0x00,
            Char  = 0x2,
            Short = 0x4,
            Int   = 0x6,
            Long  = 0x8,
            Float = 0x100,
            Double = 0x200,
            Signed = 0x10000,
            UnSigned = 0x20000,
            UChar = UnSigned | Char,
            UShort = UnSigned |Short,
            UInt = UnSigned | Int,
            ULong = UnSigned | Long,
            UnDefine = 0x30000,
        };
        Kind Knd;
    public:
        static std::shared_ptr<BuildInType> GetBuildInType(Kind kind);
        BuildInType(Kind knd,int size,int align,const char * alias) : Type(TypeClass::BInType, size, align,alias), Knd(knd) {}
        Kind GetKind() const{
            return Knd;
        }
        std::shared_ptr<Type> GetBaseType() ;
    };

    class  PointerType : public Type{
    public:
        std::shared_ptr<Type> Base;
        PointerType(std::shared_ptr<Type> base ) : Type(TypeClass::PtrType, 8, 8,"u64"), Base(base) {}

        std::shared_ptr<Type> GetBaseType();
    };


    struct Param{
        std::shared_ptr<Type> Type;
        std::shared_ptr<Token> TToken;
    };

    class FunctionType : public Type{
    private:
        std::shared_ptr<Type> ReturnType;
    public:
        std::list<std::shared_ptr<Param>> Params;
        FunctionType(std::shared_ptr<Type> returnType)  : Type(TypeClass::FuncType, 8, 8,"func"), ReturnType(returnType) {}

        std::shared_ptr<Type> GetBaseType();
    };

    struct ArrayType : public Type{
    public:
        std::shared_ptr<Type> ElementType;
        int ArrayLen;
        ArrayType(std::shared_ptr<Type> elementType,int len) :
        Type(TypeClass::AryType,len * elementType->Size,elementType ->Align,"u64"),ElementType(elementType),ArrayLen(len){}

        std::shared_ptr<Type> GetBaseType();
    };

    struct Field {
    public:
        std::shared_ptr<Type> type;
        std::shared_ptr<Token> token;
        int Offset;
        Field(std::shared_ptr<Type> ty, std::shared_ptr<Token> tok,int offset) : type(ty),token(tok),Offset(offset){}
    };

    struct RecordType : public Type{
    public:
        enum class TagKind{
            Struct,
            Union,
        };
        std::shared_ptr<Field> GetField(std::string_view fieldName);
        TagKind Kind;
        std::list<std::shared_ptr<Field>> fields;
        RecordType() : Type(TypeClass::RecordType,1,1,"u64"){}

        std::shared_ptr<Type> GetBaseType();
    };

    struct AliasType : public  Type{
    public:
        std::shared_ptr<Type> Base;
        std::shared_ptr<Token> token;
        std::shared_ptr<Type> GetBaseType();
        AliasType(std::shared_ptr<Type> typ,std::shared_ptr<Token> tk) : Type(TypeClass::AliasType,typ ->Size,typ->Align,typ->Alias) ,Base(typ),token(tk){}
    };
}

#endif //BODDY_TYPE_H
