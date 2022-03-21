//
// Created by qiaojinxia on 2022/3/20.
//

#include <memory>
#include "Lexer.h"
#include <list>
#ifndef BODDY_TYPE_H
#define BODDY_TYPE_H


namespace BDD{
    class BuildInType;
    class FunctionType;
    class PointerType;

    class Type {
    public:
        static std::shared_ptr<BuildInType> IntType;
        enum class TypeClass{
            BuildInType,
            PointerType,
            FunctionType,
        };
    private:
        int Size;
        int Align;
        TypeClass TypeC;

    public:
        virtual ~Type(){};
        Type(TypeClass tc,int size,int align) : TypeC(tc) , Size(size),Align(align){};
        bool IsIntegerType() const;
        bool IsFunctionType() const;
        bool IsPointerType() const;
    };


    class BuildInType: public Type{
    public:
        enum class Kind{
            Int,
        };
        Kind Knd;
    public:
        BuildInType(Kind knd,int size,int align) : Type(TypeClass::BuildInType,size,align),Knd(knd) {}
        Kind GetKind() const{
            return Knd;
        }
    };

    class  PointerType : public Type{
    private:
        std::shared_ptr<Type> Base;
    public:
        PointerType(std::shared_ptr<Type> base)  : Type(TypeClass::PointerType,8,8),Base(base) {}
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
        FunctionType(std::shared_ptr<Type> returnType)  : Type(TypeClass::FunctionType,8,8),ReturnType(returnType) {}
    };


}

#endif //BODDY_TYPE_H
