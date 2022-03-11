//
// Created by a on 2022/3/9.
//

#include "AstNode.h"

using namespace BDD;


void ConstantNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void BinaryNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}

void ProgramNode::Accept(AstVisitor *visitor) {
    visitor->Visitor(this);
}
