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
    class Type {
    public:
        static std::shared_ptr<BuildInType> IntType;
        static std::shared_ptr<PointerType> Pointer;
        enum class TypeClass{
            BInType,
            PtrType,
            FuncType,
            AryType,
        };
        int Size;
        int Align;
    private:
        TypeClass TypeC;

    public:
        virtual ~Type(){};
        Type(TypeClass tc,int size,int align) : TypeC(tc) , Size(size),Align(align){};
        bool IsIntegerType() const;
        bool IsFunctionType() const;
        bool IsPointerType() const;
        bool IsArrayType() const;
    };


    class BuildInType: public Type{
    public:
        enum class Kind{
            Int,
            Pointer,
        };
        Kind Knd;
    public:
        BuildInType(Kind knd,int size,int align) : Type(TypeClass::BInType, size, align), Knd(knd) {}
        Kind GetKind() const{
            return Knd;
        }
    };

    class  PointerType : public Type{
    public:
        std::shared_ptr<Type> Base;
        PointerType(std::shared_ptr<Type> base)  : Type(TypeClass::PtrType, 8, 8), Base(base) {}
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
        FunctionType(std::shared_ptr<Type> returnType)  : Type(TypeClass::FuncType, 8, 8), ReturnType(returnType) {}
    };

    struct ArrayType : public Type{
    public:
        std::shared_ptr<Type> ElementType;
        int ArrayLen;
        ArrayType(std::shared_ptr<Type> elementType,int len) :
        Type(TypeClass::AryType,len * elementType->Size,elementType ->Align),ElementType(elementType),ArrayLen(len){}
    };

}

#endif //BODDY_TYPE_H
