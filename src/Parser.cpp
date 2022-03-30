//
// Created by a on 2022/3/8.
//

#include "Parser.h"
#include "Lexer.h"
#include "AstNode.h"
#include "Diag.h"
#include "TypeVisitor.h"

using namespace BDD;

std::shared_ptr<AstNode> Parser::ParseBinaryExpr(std::shared_ptr<AstNode> left) {
    auto node = std::make_shared<BinaryNode>();
    BinaryOperator anOperator ;
    std::shared_ptr<BinaryNode> prevNode = std::dynamic_pointer_cast<BinaryNode >(left);
    std::shared_ptr<AstNode> leftNode ;
    if (prevNode){
        leftNode = ParseUnaryExpr();
    }
    switch (Lex.CurrentToken -> Kind) {
        case TokenKind::Plus:
            anOperator = BinaryOperator::Add;
            break;
        case TokenKind::Minus:
            anOperator = BinaryOperator::Sub;
            break;
        case TokenKind::Start:
            anOperator = BinaryOperator::Mul;
            break;
        case TokenKind::Slash:
            anOperator = BinaryOperator::Div;
            break;
        case TokenKind::Assign:
            anOperator = BinaryOperator::Assign;
            break;
        case TokenKind::Mod:
            anOperator = BinaryOperator::Mod;
            break;
        case TokenKind::Greater:
            anOperator = BinaryOperator::Greater;
            break;
        case TokenKind::GreaterEqual:
            anOperator = BinaryOperator::GreaterEqual;
            break;
        case TokenKind::Lesser:
            anOperator = BinaryOperator::Lesser;
            break;
        case TokenKind::LesserEqual:
            anOperator = BinaryOperator::LesserEqual;
            break;
        case TokenKind::Equal:
            anOperator = BinaryOperator::Equal;
            break;
        case TokenKind::NotEqual:
            anOperator = BinaryOperator::NotEqual;
            break;
        default:
            LastOperation = BinaryOperator::Eof;
            return leftNode;
    }
    if (leftNode)
        node -> Lhs = leftNode;
    else
        node -> Lhs = left;
    if (prevNode) {
        auto preOpt = OpPrecedence[prevNode->BinOp];
        auto noeOpt = OpPrecedence[anOperator];
        if (preOpt < noeOpt){
            LastOperation = anOperator;
            return leftNode;
        }else if (preOpt == noeOpt){
            Lex.GetNextToken();
            node -> Lhs = prevNode -> Lhs;
            node -> Rhs = leftNode;
            node -> BinOp = prevNode -> BinOp;
            prevNode -> BinOp = anOperator;
            prevNode -> Lhs = node;
            auto subNode =  ParseBinaryExpr(prevNode);
            return subNode;
        }
    }
    node -> BinOp = anOperator;
    Lex.GetNextToken();
    node  -> Rhs = ParseBinaryExpr(node);
    while (OpPrecedence[node -> BinOp] >= OpPrecedence[LastOperation] && LastOperation != BinaryOperator::Eof){
        auto newNode = std::make_shared<BinaryNode>();
        newNode -> BinOp = LastOperation;
        newNode -> Lhs = node -> Lhs;
        newNode -> BinOp = LastOperation;
        Lex.GetNextToken();
        newNode -> Rhs  =  ParseBinaryExpr(newNode);
        node -> Lhs = newNode;
    }
    return node;

}

std::shared_ptr<AstNode> Parser::ParsePrimaryExpr() {
    auto node = std::make_shared<AstNode>();
    switch (Lex.CurrentToken -> Kind){
        case TokenKind::LParent:
        {
            Lex.BeginPeekToken();
            Lex.GetNextToken();
            if (Lex.CurrentToken -> Kind == TokenKind::LBrace){
                Lex.EndPeekToken();
                Lex.ExceptToken(TokenKind::LParent);
                Lex.ExceptToken(TokenKind::LBrace);
                auto node = std::make_shared<StmtExprNode>();
                while (Lex.CurrentToken -> Kind != TokenKind::RBrace){
                    node -> Stmts.push_back(ParseStatement());
                }
                Lex.GetNextToken();
                Lex.ExceptToken(TokenKind::RParent);
                return node;
            }
            Lex.EndPeekToken();
            Lex.GetNextToken();
            node = ParseExpr();
            Lex.ExceptToken(TokenKind::RParent);
            break;
        }
        case TokenKind::Identifier:
        {
            Lex.BeginPeekToken();
            Lex.GetNextToken();
            if (Lex.CurrentToken -> Kind == TokenKind::LParent){
                Lex.EndPeekToken();
                return ParseFuncCallNode();
            }
            Lex.EndPeekToken();

            auto exprVarNode = std::make_shared<ExprVarNode>();
            exprVarNode -> Name = Lex.CurrentToken->Content;
            auto obj = FindLocalVar(Lex.CurrentToken -> Content);
            if (!obj){
                obj = NewLocalVar(Lex.CurrentToken -> Content,Type::IntType);
            }
            exprVarNode ->VarObj = obj;
            node = exprVarNode;
            Lex.GetNextToken();
            break;
        }
       case TokenKind::Num:
       {
           auto constNode = std::make_shared<ConstantNode>();
           constNode -> Value = Lex.CurrentToken -> Value;
           Lex.GetNextToken();
           node =  constNode;
           break;
       }
        case TokenKind::SizeOf:
        {
            Lex.GetNextToken();
            auto sizeOfNode = std::make_shared<SizeOfExprNode>();
            sizeOfNode -> Lhs = ParseUnaryExpr();
            node =  sizeOfNode;
            break;
        }
       default:
           DiagE(Lex.SourceCode,Lex.CurrentToken->Location.Line,Lex.CurrentToken->Location.Col,"not support type");
    }
    return node;
}

std::shared_ptr<AstNode> Parser::ParseExpr() {
    std::shared_ptr<AstNode> left = ParseUnaryExpr();;
    while (Lex.CurrentToken -> Kind != TokenKind::Semicolon && Lex.CurrentToken -> Kind != TokenKind::RParent
    && Lex.CurrentToken -> Kind != TokenKind::Comma ) {
        left = ParseBinaryExpr(left);
    }
    TypeVisitor typeVisitor;
    left -> Accept( &typeVisitor);
    return left;
}


std::shared_ptr<ProgramNode> Parser::Parse() {
    auto node = std::make_shared<ProgramNode>();
    while (Lex.CurrentToken -> Kind != TokenKind::Eof){
        node ->Funcs.push_back(ParseFunc());
    }
    return node;
}

std::shared_ptr<AstNode> Parser::ParseStatement() {
    if (Lex.CurrentToken -> Kind == TokenKind::If){
        auto node = std::make_shared<IfStmtNode>();
        Lex.GetNextToken();
        Lex.ExceptToken(TokenKind::LParent);
        node ->Cond = ParseExpr();
        Lex.ExceptToken(TokenKind::RParent);
        node -> Then = ParseStatement();
        if (Lex.CurrentToken -> Kind == TokenKind::Else){
            Lex.GetNextToken();
            node -> Else = ParseStatement();
        }
        return node;
    }else if (Lex.CurrentToken -> Kind == TokenKind::LBrace){
        auto node = std::make_shared<BlockStmtNode>();
        Lex.GetNextToken();
        while (Lex.CurrentToken->Kind != TokenKind::RBrace){
            node -> Stmts.push_back(ParseStatement());
        }
        Lex.ExceptToken(TokenKind::RBrace);
        return node;
    }else if (Lex.CurrentToken -> Kind == TokenKind::While){
        auto node = std::make_shared<WhileStmtNode>();
        Lex.GetNextToken();
        Lex.ExceptToken(TokenKind::LParent);
        node -> Cond = ParseExpr();
        Lex.ExceptToken(TokenKind::RParent);
        node -> Then = ParseStatement();
        return node;
    }else if (Lex.CurrentToken -> Kind == TokenKind::Do){
        auto node = std::make_shared<DoWhileStmtNode>();
        Lex.GetNextToken();
        node -> Stmt = ParseStatement();
        Lex.ExceptToken(TokenKind::While);
        Lex.ExceptToken(TokenKind::LParent);
        node -> Cond = ParseExpr();
        Lex.ExceptToken(TokenKind::RParent);
        return node;
    } else if (Lex.CurrentToken -> Kind == TokenKind::For){
        auto node = std::make_shared<ForStmtNode>();
        Lex.GetNextToken();
        Lex.ExceptToken(TokenKind::LParent);
        if (Lex.CurrentToken->Kind != TokenKind::Semicolon){
            node -> Init = ParseExpr();
            Lex.ExceptToken(TokenKind::Semicolon);
            if (Lex.CurrentToken -> Kind !=TokenKind::Semicolon)
                node -> Cond = ParseExpr();
            Lex.ExceptToken(TokenKind::Semicolon);
            if (Lex.CurrentToken -> Kind != TokenKind::RParent)
                node -> Inc = ParseExpr();
            Lex.ExceptToken(TokenKind::RParent);
            node -> Stmt = ParseStatement();
            return node;
        }
    }else if (Lex.CurrentToken-> Kind == TokenKind::Return){
        auto node = std::make_shared<ReturnStmtNode>();
        Lex.GetNextToken();
        node -> Lhs = ParseExpr();
        Lex.ExceptToken(TokenKind::Semicolon);
        return node;
    }else if (IsTypeName()){
        std::list<std::shared_ptr<ExprVarNode>> declarationNodes;
        auto tokens = std::list<std::shared_ptr<Token>>();
        auto type = ParseDeclarator(ParseDeclarationSpec(),&tokens);
        for (auto &tk:tokens) {
            auto newVarNode = std::make_shared<ExprVarNode>();
            newVarNode -> Name = tk -> Content;
            newVarNode -> VarObj =  NewLocalVar(newVarNode ->Name, type);
            declarationNodes.push_back(newVarNode);
        }
        if (Lex.CurrentToken -> Kind == TokenKind::Semicolon){
            auto multiDeclarationStmtNode = std::make_shared<DeclarationStmtNode>();
            multiDeclarationStmtNode -> declarationNodes = declarationNodes;
            multiDeclarationStmtNode ->Type = type;
            return multiDeclarationStmtNode;
        }
        auto multiAssignNode = std::make_shared<DeclarationAssignmentStmtNode>();
        std::list<std::shared_ptr<BinaryNode>> assignNodes;
        for (auto &dn:declarationNodes) {
            auto assignNode = std::make_shared<BinaryNode>();
            assignNode -> Lhs = dn;
            assignNode -> BinOp = BinaryOperator::Assign;
            assignNodes.push_back(assignNode);
        }
        Lex.ExceptToken( TokenKind::Assign);
        auto valueNode = ParseUnaryExpr();
        for (auto &n:assignNodes) {
           n ->Rhs = valueNode;
        }
        Lex.ExceptToken(TokenKind::Semicolon);
        multiAssignNode ->AssignNodes = assignNodes;
        return multiAssignNode;
    }
    auto node = std::make_shared<ExprStmtNode>();
        if (Lex.CurrentToken -> Kind != TokenKind::Semicolon){
        node -> Lhs = ParseExpr();
    }
    Lex.ExceptToken(TokenKind::Semicolon);
    return node;
}

std::shared_ptr<Var> Parser::FindLocalVar(std::string_view varName) {
    if (LocalsMap.find(varName) != LocalsMap.end()){
        return LocalsMap[varName];
    }
    return nullptr;
}

std::shared_ptr<Var> Parser::NewLocalVar(std::string_view varName,std::shared_ptr<Type> type) {
    auto obj = std::make_shared<Var>();
    obj -> Type = type;
    obj ->Name = varName;
    obj -> Offset = 0;
    LocalVars -> push_front(obj);
    LocalsMap[varName] = obj;
    return obj;
}

std::shared_ptr<AstNode> Parser::ParseFunc() {
    auto node =std::make_shared<FunctionNode>();
    LocalVars = &node -> Locals;
    LocalsMap.clear();

    auto type = ParseDeclarationSpec();
    std::list<std::shared_ptr<Token>> nameTokens;
    node -> FuncName = Lex.CurrentToken->Content;

    type = ParseDeclarator(type,&nameTokens);

    node -> Type = type;

    std::shared_ptr<FunctionType> funcType = std::dynamic_pointer_cast<FunctionType>(type);
    if (funcType != nullptr){
        for(auto it = funcType -> Params.rbegin();it != funcType -> Params.rend();++it){
            node ->Params.push_front(NewLocalVar( (*it) ->TToken ->Content,(*it) ->Type));
        }
    }

    Lex.ExceptToken(TokenKind::LBrace);
    while (Lex.CurrentToken -> Kind != TokenKind::RBrace){
        node -> Stmts.push_back(ParseStatement());
    }
    Lex.ExceptToken(TokenKind::RBrace);
    return node;
}

std::shared_ptr<AstNode> Parser::ParseFuncCallNode() {
    auto node=std::make_shared<FuncCallNode>();
    node -> FuncName = Lex.CurrentToken -> Content;
    Lex.ExceptToken(TokenKind::Identifier);
    Lex.ExceptToken(TokenKind::LParent);
    if (Lex.CurrentToken -> Kind!= TokenKind::RParent){
        node -> Args.push_back(ParseExpr());
        while(Lex.CurrentToken -> Kind == TokenKind::Comma){
            Lex.GetNextToken();
            node -> Args.push_back(ParseUnaryExpr());
        }
    }
    Lex.ExceptToken(TokenKind::RParent);
    return node;
}

std::shared_ptr<Type> Parser::ParseDeclarationSpec() {
    if (Lex.CurrentToken -> Kind == TokenKind::Int){
        Lex.GetNextToken();
        return Type::IntType;
    }
    DiagE(Lex.SourceCode,Lex.CurrentToken->Location.Line,Lex.CurrentToken->Location.Col,"type not support current!");
    return nullptr;
}

std::shared_ptr<Type> Parser::ParseTypeSuffix(std::shared_ptr<Type> baseType) {
    if (Lex.CurrentToken -> Kind == TokenKind::LParent){
        auto funcType = std::make_shared<FunctionType>(baseType);
        Lex.GetNextToken();
        if (Lex.CurrentToken -> Kind != TokenKind::RParent){
            std::list<std::shared_ptr<Token>> tokens;
            auto type = ParseDeclarator(ParseDeclarationSpec(),&tokens);
            auto param = std::make_shared<Param>();
            param ->Type = type;
            param -> TToken = tokens.front();
            funcType -> Params.push_back(param);
            while (Lex.CurrentToken -> Kind != TokenKind::RParent){
                Lex.ExceptToken(TokenKind::Comma);
                auto type = ParseDeclarator(ParseDeclarationSpec(),&tokens);
                auto param = std::make_shared<Param>();
                param ->Type = type;
                param ->TToken = tokens.front();
                funcType ->  Params.push_back(param);
            }
        }
        Lex.ExceptToken(TokenKind::RParent);
        return funcType;
    }else if(Lex.CurrentToken -> Kind == TokenKind::LBracket){
        Lex.GetNextToken();
        int num = Lex.CurrentToken -> Value;
        Lex.ExceptToken(TokenKind::Num);
        Lex.ExceptToken(TokenKind::RBracket);
        auto type = ParseTypeSuffix(baseType);
        return std::make_shared<ArrayType>(type,num);
    }
  return baseType;
}

std::shared_ptr<Type> Parser::ParseDeclarator(std::shared_ptr<Type> baseType, std::list<std::shared_ptr<Token>> *nameTokens) {
    auto type = baseType;
    while(Lex.CurrentToken->Kind == TokenKind::Start){
        type = std::make_shared<PointerType>(type);
        Lex.GetNextToken();
    }
    if (Lex.CurrentToken->Kind != TokenKind::Identifier){
        DiagE(Lex.SourceCode,Lex.CurrentToken->Location.Line,Lex.CurrentToken->Location.Col,"except variable name!");
    }
    while(Lex.CurrentToken -> Kind == TokenKind::Identifier){
        (*nameTokens).push_back(Lex.CurrentToken);
        Lex.SkipToken(TokenKind::Comma);
    }
    return ParseTypeSuffix(type);
}

bool Parser::IsTypeName() {
    if (Lex.CurrentToken -> Kind == TokenKind::Int)
        return true;
    return false;
}

std::shared_ptr<AstNode> Parser::ParseUnaryExpr() {
    if (Lex.CurrentToken -> Kind == TokenKind::Plus || Lex.CurrentToken->Kind  == TokenKind::Minus
    || Lex.CurrentToken->Kind  == TokenKind::Start || Lex.CurrentToken->Kind  == TokenKind::Amp){
        auto node = std::make_shared<UnaryNode>();
        switch (Lex.CurrentToken -> Kind){
            case TokenKind::Plus:
                node -> Uop = UnaryOperator::Plus;
                break;
            case TokenKind::Minus:
                node -> Uop = UnaryOperator::Minus;
                break;
            case TokenKind::Start:
                node -> Uop = UnaryOperator::Deref;
                break;
            case TokenKind::Amp:
                node -> Uop = UnaryOperator::Amp;
                break;
            default:
                break;
        }
        Lex.GetNextToken();
        node -> Lhs = ParseUnaryExpr();
        return node;
    }
    return ParsePrimaryExpr();
}



