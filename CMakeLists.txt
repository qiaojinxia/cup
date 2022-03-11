cmake_minimum_required(VERSION 3.21)
project(boddy)

set(CMAKE_CXX_STANDARD 17)

add_executable(boddy main.cpp Lexer.cpp Lexer.h Parser.cpp Parser.h CodeGenerate.cpp CodeGenerate.h AstNode.cpp AstNode.h PrintVisitor.cpp PrintVisitor.h)
