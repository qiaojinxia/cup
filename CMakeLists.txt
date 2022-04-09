cmake_minimum_required(VERSION 3.20)
project(boddy)

set(CMAKE_CXX_STANDARD 17)

add_executable(boddy
        src/main.cpp
        src/Lexer.cpp src/Lexer.h
        src/Parser.cpp src/Parser.h
        src/CodeGenerate.cpp src/CodeGenerate.h
        src/AstNode.cpp src/AstNode.h
        src/PrintVisitor.cpp src/PrintVisitor.h
        src/Diag.cpp src/Diag.h
        src/Type.cpp src/Type.h src/TypeVisitor.cpp src/TypeVisitor.h test/testFunc.c src/Scope.cpp src/Scope.h src/Common.cpp src/Common.h)
