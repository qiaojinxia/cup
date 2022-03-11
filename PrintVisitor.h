//
// Created by a on 2022/3/9.
//

#ifndef BODDY_PRINTVISITOR_H
#define BODDY_PRINTVISITOR_H

#include "AstNode.h"

namespace BDD{
    class PrintVisitor: public AstVisitor{
    public:
        PrintVisitor(){}
        void Visitor(ProgramNode *node) override;
    private:
        void Visitor(BinaryNode *node) override;
        void Visitor(ConstantNode *node) override;
    };

}
#endif //BODDY_PRINTVISITOR_H
