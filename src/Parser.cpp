//
// Created by a on 2022/3/8.
//

#include <string>
#include "Parser.h"
#include "Lexer.h"
#include "AstNode.h"
#include "Diag.h"

using namespace BDD;

// primary = "(" "{" stmt+ "}" ")"
//         | "(" expr ")"
//         | "sizeof" "(" type-name ")"
//         | "sizeof" unary
//         | "_Alignof" "(" type-name ")"
//         | "_Alignof" unary
//         | ident func-args?
//         | str
//         | num
//

std::shared_ptr<AstNode> Parser::ParsePrimaryExpr() {
    auto node = std::make_shared<AstNode>();
    switch (Lex.CurrentToken -> Kind){
        case TokenKind::LParent:
        {
            Lex.BeginPeekToken();
            Lex.GetNextToken();
            if (Lex.CurrentToken -> Kind == TokenKind::LBrace){
                Scope::GetInstance() -> PushScope();
                Lex.EndPeekToken();
                Lex.ExceptToken(TokenKind::LParent);
                Lex.ExceptToken(TokenKind::LBrace);
                auto node = std::make_shared<StmtExprNode>();
                while (Lex.CurrentToken -> Kind != TokenKind::RBrace){
                    node -> Stmts.push_back(ParseStatement());
                }
                Lex.GetNextToken();
                Lex.ExceptToken(TokenKind::RParent);
                Scope::GetInstance() -> PopScope();
                return node;
            }
            Lex.EndPeekToken();
            Lex.GetNextToken();
            node = ParseExpr();
            Lex.ExceptToken(TokenKind::RParent);
            break;
        }
        case TokenKind::LBrace:
        {
            Lex.BeginPeekToken();
            Lex.GetNextToken();
            if (Lex.CurrentToken ->Kind == TokenKind::Num || Lex.CurrentToken ->Kind == TokenKind::String){
                auto initValues  = ParseInitListExpr();
                node = initValues;
            }else{
                Lex.EndPeekToken();
            }
        }
            break;
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

            auto varName = std::string (Lex.CurrentToken -> Content);
            auto obj = FindLocalVar(Lex.CurrentToken -> Content);
            if (!obj){
                DiagLoc(Lex.SourceCode,Lex.GetLocation(),"undefined variable %s ",varName.data());
            }
            exprVarNode ->VarObj = obj;
            node = exprVarNode;
            Lex.GetNextToken();
            break;
        }
       case TokenKind::Num:
       {
           auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
           constNode -> Value = Lex.CurrentToken -> Value;
           constNode -> Type = Type::IntType;
           Lex.GetNextToken();
           node =  constNode;
           break;
       }
        case TokenKind::String:
        {
            auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            constNode -> Value = Lex.CurrentToken -> Value;
            constNode -> Type = Type::StringType;
            Lex.GetNextToken();
            node =  constNode;
            break;
        }
       case TokenKind::FloatNum:
        {
            auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            constNode -> Value = Lex.CurrentToken -> Value;
            constNode -> Type = Type::FloatType;
            Lex.GetNextToken();
            node =  constNode;
            break;
        }
        case TokenKind::SizeOf:
        {   //Accept type or var sizeof(int) |sizeof a
            Lex.GetNextToken();
            auto sizeOfNode = std::make_shared<SizeOfExprNode>();
            if (Lex.CurrentToken ->Kind == TokenKind::LParent){
                if (IsTypeName()){
                    Lex.GetNextToken();
                    auto type = ParseDeclarationSpec(0);
                    auto emptyNode= std::make_shared<EmptyNode>();
                    emptyNode ->Type = type;
                    sizeOfNode -> Lhs = emptyNode;
                    Lex.ExceptToken(TokenKind::RParent);
                }else{
                    sizeOfNode -> Lhs = ParseCastExpr();
                }
            }else{
                sizeOfNode -> Lhs = ParsePrimaryExpr();
            }

            node =  sizeOfNode;
            break;
        }
       default:
            DiagLoc(Lex.SourceCode,Lex.GetLocation(),"not support type",Lex.CurrentToken->Kind);
    }
    return node;
}

std::shared_ptr<AstNode> Parser::ParseExpr() {
    auto declarationNode = ParseDeclarationExpr();
    if (declarationNode){
        return declarationNode;
    }
    auto binaryNode = ParseBinaryExpr(16);
    if (Lex.CurrentToken ->Kind == TokenKind::Semicolon ){
        return binaryNode;
    }
    return binaryNode;
}

std::shared_ptr<ProgramNode> Parser::Parse() {
    Scope::GetInstance() -> PushScope();
    auto node = std::make_shared<ProgramNode>();
    while (Lex.CurrentToken -> Kind != TokenKind::Eof){
        node ->Funcs.push_back(ParseFunc());
    }
    Scope::GetInstance() -> PopScope();
    return node;
}


std::shared_ptr<AstNode> Parser::ParseStatement() {
    if (Lex.CurrentToken -> Kind == TokenKind::If){
        auto node = std::make_shared<IfStmtNode>();
        Lex.GetNextToken();
        Lex.ExceptToken(TokenKind::LParent);
        node ->Cond = ParseExpr();
        Lex.ExceptToken(TokenKind::RParent);
        Lex.ExceptToken(TokenKind::LBrace);
        node -> Then = ParseStatement();
        Lex.ExceptToken(TokenKind::RBrace);
        if (Lex.CurrentToken -> Kind == TokenKind::Else){
            Lex.GetNextToken();
            Lex.ExceptToken(TokenKind::LBrace);
            if (Lex.CurrentToken -> Kind != TokenKind::RBrace){
                node -> Else = ParseStatement();
            }
            Lex.ExceptToken(TokenKind::RBrace);
        }
        return node;
    }else if (Lex.CurrentToken -> Kind == TokenKind::LBrace){
        Scope::GetInstance() -> PushScope();
        auto node = std::make_shared<BlockStmtNode>();
        Lex.GetNextToken();
        while (Lex.CurrentToken->Kind != TokenKind::RBrace){
            node -> Stmts.push_back(ParseStatement());
        }
        Lex.ExceptToken(TokenKind::RBrace);
        Scope::GetInstance() -> PopScope();
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
    }else if (Lex.CurrentToken-> Kind == TokenKind::Break){
        Lex.GetNextToken();
        auto node = std::make_shared<BreakStmtNode>();
        Lex.ExceptToken(TokenKind::Semicolon);
        return node;
    }else if (Lex.CurrentToken-> Kind == TokenKind::Continue){
        Lex.GetNextToken();
        auto node = std::make_shared<ContinueStmtNode>();
        Lex.ExceptToken(TokenKind::Semicolon);
        return node;
    }else if (Lex.CurrentToken -> Kind == TokenKind::TypeDef){
        //"typedef" ("char" | "short" | "int" | "long" | "float" | "double" | "struct{" (field1,)+ "}")+ aliasName
        Lex.GetNextToken();
        auto tokens = std::list<std::shared_ptr<Token>>();
        auto type = ParseDeclarator(ParseDeclarationSpec(0),&tokens);
        if(Scope::GetInstance() -> FindTag( Lex.CurrentToken->Content)){
            DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"typedef repeated!");
        }
        for (auto token:tokens) {
            auto newType = std::make_shared<AliasType>(type,token);
            Scope::GetInstance() ->PushTag(token->Content,newType);
        }
        return std::make_shared<EmptyNode>();
    }
    auto node = std::make_shared<ExprStmtNode>();
    if (Lex.CurrentToken -> Kind != TokenKind::Semicolon)
        node -> Lhs = ParseExpr();
    Lex.ExceptToken(TokenKind::Semicolon);
    return node;
}

std::shared_ptr<Var> Parser::FindLocalVar(std::string_view varName) {
    return Scope::GetInstance() -> FindVar(varName);
}


std::shared_ptr<Var> Parser::NewLocalVar(std::string_view varName,std::shared_ptr<Type> type) {
    auto obj = std::make_shared<Var>();
    obj -> Type = type;
    obj ->Name = varName;
    obj -> Offset = 0;
    LocalVars -> push_front(obj);
    Scope::GetInstance() -> PushVar(obj);
    return obj;
}

std::shared_ptr<AstNode> Parser::ParseFunc() {
    auto node =std::make_shared<FunctionNode>();
    LocalVars = &node -> Locals;
    Scope::GetInstance() -> PushScope();
    auto type = ParseDeclarationSpec(0);
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
    Scope::GetInstance() -> PopScope();
    Lex.ExceptToken(TokenKind::RBrace);
    return node;
}
//ParseFuncCallNode ::=
std::shared_ptr<AstNode> Parser::ParseFuncCallNode() {
    auto node=std::make_shared<FuncCallNode>();
    node -> FuncName = Lex.CurrentToken -> Content;
    Lex.ExceptToken(TokenKind::Identifier);
    Lex.ExceptToken(TokenKind::LParent);
    if (Lex.CurrentToken -> Kind!= TokenKind::RParent){
        node -> Args.push_back(ParseExpr());
        while(Lex.CurrentToken -> Kind == TokenKind::Comma){
            Lex.GetNextToken();
            node -> Args.push_back(ParseExpr());
        }
    }
    Lex.ExceptToken(TokenKind::RParent);
    return node;
}

//ParseDeclarationSpec :=  ( int ｜ char | short | long | union | struct | union | float | double | *)  ParseDeclarationSpec |  ε
std::shared_ptr<Type> Parser::ParseDeclarationSpec(int baseType) {
    std::shared_ptr<Type> sType;
    while(true){
        if (Lex.CurrentToken -> Kind == TokenKind::Int){
            Lex.GetNextToken();
            baseType += (int) BuildInType::Kind::Int;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Char){
            Lex.GetNextToken();
            baseType += (int) BuildInType::Kind::Char;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Short){
            Lex.GetNextToken();
            baseType += (int) BuildInType::Kind::Short;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Long){
            Lex.GetNextToken();
            baseType += (int) BuildInType::Kind::Long;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Struct){
            Lex.GetNextToken();
            sType = ParseStructDeclaration();
           continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Union){
            Lex.GetNextToken();
            sType =  ParseUnionDeclaration();
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Float){
            Lex.GetNextToken();
            baseType += (int) BuildInType::Kind::Float;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Double){
            Lex.GetNextToken();
            baseType += (int) BuildInType::Kind::Double;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::_Bool){
            Lex.GetNextToken();
            baseType += (int) BuildInType::Kind::Bool;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Asterisk){
            Lex.GetNextToken();
            std::shared_ptr<PointerType> pointerType;
            if (sType){
                pointerType = std::make_shared<PointerType>(sType);
            }else{
                pointerType = std::make_shared<PointerType>(ParseDeclarationSpec(baseType));
            }
           return pointerType;
        }else if(Lex.CurrentToken -> Kind == TokenKind::SIGNED){
            Lex.GetNextToken();
            baseType |= (int) BuildInType::Kind::Signed;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::UNSIGNED){
            Lex.GetNextToken();
            baseType |= (int) BuildInType::Kind::UnSigned;
            continue;
        }else{
            auto type = Scope::GetInstance() -> FindTag(Lex.CurrentToken ->Content);
            if (type){
                Lex.GetNextToken();
                return type;
            }
            break;
        }
    }
    if (sType)
        return sType;
    std::shared_ptr<Type> type;
    switch ((BuildInType::Kind)baseType) {
        case BuildInType::Kind::Void:
            type = Type::VoidType;
            break;
        case BuildInType::Kind::Char:
        case BuildInType::Kind::Signed + BuildInType::Kind::Char:
            type = Type::CharType;
            break;
        case BuildInType::Kind::UnSigned + BuildInType::Kind::Char:
            type = Type::UIntType;
            break;
        case BuildInType::Kind::Short:
        case BuildInType::Kind::Short + BuildInType::Kind::Int:
        case BuildInType::Kind::Signed + BuildInType::Kind::Short:
        case BuildInType::Kind::Signed + BuildInType::Kind::Short + BuildInType::Kind::Int:
            type = Type::ShortType;
            break;
        case BuildInType::Kind::UnSigned + BuildInType::Kind::Short:
        case BuildInType::Kind::UnSigned +  BuildInType::Kind::Short + BuildInType::Kind::Int:
            type = Type::UShortType;
            break;
        case BuildInType::Kind::Int:
        case BuildInType::Kind::Signed:
        case BuildInType::Kind::Signed + BuildInType::Kind::Int:
            type = Type::IntType;
            break;
        case BuildInType::Kind::UnSigned:
        case BuildInType::Kind::UnSigned + BuildInType::Kind::Int:
            type = Type::UIntType;
            break;
        case BuildInType::Kind::Long:
        case BuildInType::Kind::Long + BuildInType::Kind::Int:
        case BuildInType::Kind::Long + BuildInType::Kind::Long:
        case BuildInType::Kind::Long + BuildInType::Kind::Long + BuildInType::Kind::Int:
        case BuildInType::Kind::Signed + BuildInType::Kind::Long:
        case BuildInType::Kind::Signed + BuildInType::Kind::Long + BuildInType::Kind::Int:
        case BuildInType::Kind::Signed + BuildInType::Kind::Long + BuildInType::Kind::Long:
        case BuildInType::Kind::Signed + BuildInType::Kind::Long + BuildInType::Kind::Long + BuildInType::Kind::Int:
            type = Type::LongType;
            break;
        case BuildInType::Kind::UnSigned + BuildInType::Kind::Long:
        case BuildInType::Kind::UnSigned + BuildInType::Kind::Long + BuildInType::Kind::Int:
        case BuildInType::Kind::UnSigned + BuildInType::Kind::Long + BuildInType::Kind::Long:
        case BuildInType::Kind::UnSigned + BuildInType::Kind::Long + BuildInType::Kind::Long + BuildInType::Kind::Int:
            type = Type::ULongType;
            break;
        case BuildInType::Kind::Float:
            type = Type::FloatType;
            break;
        case BuildInType::Kind::Double:
            type = Type::DoubleType;
            break;
        case BuildInType::Kind::Bool:
            type = Type::BoolType;
            break;
        default:
            DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"invalid type!");
    }
    return type;
}

//ParseTypeSuffix ::=  "(" (ParseDeclarationSpec,ParseDeclarator)? ")" |  ("[" ParseTypeSuffix "]")* | ε
std::shared_ptr<Type> Parser::ParseTypeSuffix(std::shared_ptr<Type> baseType) {
    if (Lex.CurrentToken -> Kind == TokenKind::LParent){
        auto funcType = std::make_shared<FunctionType>(baseType);
        Lex.GetNextToken();
        if (Lex.CurrentToken -> Kind != TokenKind::RParent){
            std::list<std::shared_ptr<Token>> tokens;
            auto type = ParseDeclarator(ParseDeclarationSpec(0),&tokens);
            auto param = std::make_shared<Param>();
            param -> Type = type;
            param -> TToken = tokens.back();
            funcType -> Params.push_back(param);
            while (Lex.CurrentToken -> Kind != TokenKind::RParent){
                auto type = ParseDeclarator(ParseDeclarationSpec(0),&tokens);
                auto param = std::make_shared<Param>();
                param ->Type = type;
                param ->TToken = tokens.back();
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

//  ParseDeclarator ::=  (Identifier)* ParseTypeSuffix
std::shared_ptr<Type> Parser::ParseDeclarator(std::shared_ptr<Type> baseType, std::list<std::shared_ptr<Token>> *nameTokens) {
    auto type = baseType;
    if (Lex.CurrentToken->Kind != TokenKind::Identifier){
        DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"except variable name!");
    }
    while(Lex.CurrentToken -> Kind == TokenKind::Identifier){
        (*nameTokens).push_back(Lex.CurrentToken);
        Lex.SkipToken(TokenKind::Comma);
    }
    return ParseTypeSuffix(type);
}

//ParseUnaryExpr ::= (+ | - | * | & )? ParseCastExpr | ParsePostFixExpr
std::shared_ptr<AstNode> Parser::ParseUnaryExpr() {
    if (Lex.CurrentToken -> Kind == TokenKind::Plus || Lex.CurrentToken->Kind  == TokenKind::Minus
        || Lex.CurrentToken->Kind  == TokenKind::Asterisk || Lex.CurrentToken->Kind == TokenKind::Amp ){
        auto node = std::make_shared<UnaryNode>();
        switch (Lex.CurrentToken -> Kind){
            case TokenKind::Plus:
                node -> Uop = UnaryOperator::Plus;
                break;
            case TokenKind::Minus:
                node -> Uop = UnaryOperator::Minus;
                break;
            case TokenKind::Asterisk:
                node -> Uop = UnaryOperator::Deref;
                break;
            case TokenKind::Amp:
                node -> Uop = UnaryOperator::Addr;
                break;
            default:
                break;
        }
        Lex.GetNextToken();
        node -> Lhs = ParseCastExpr();
        return node;
    }
    return ParsePostFixExpr();
}


//ParsePostFixExpr ::= ParsePrimaryExpr ("++" | "--" ｜ "->" ident | "." ident ｜ "[" ParseExpr "]" )*
std::shared_ptr<AstNode> Parser::ParsePostFixExpr() {
    auto left = ParsePrimaryExpr();
    if (auto constNode = std::dynamic_pointer_cast<ConstantNode>(left)){
        Scope::GetInstance() -> PutToConstantTable(constNode);
    }
    while (true){
        if (Lex.CurrentToken -> Kind == TokenKind::LParent){
            return ParseFuncCallNode();
        }else if (Lex.CurrentToken -> Kind == TokenKind::LBracket){
            Lex.GetNextToken();
            auto starNode = std::make_shared<ArefNode>();
            starNode -> Offset = ParseExpr();
            starNode -> Lhs = left;
            Lex.ExceptToken(TokenKind::RBracket);
            left = starNode;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Period){
            auto memberNode = std::make_shared<MemberAccessNode>();
            Lex.GetNextToken();
            memberNode -> Lhs = left;
            memberNode -> fieldName = Lex.CurrentToken -> Content;
            left = memberNode;
            Lex.ExceptToken(TokenKind::Identifier);
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::PointerTo){
            Lex.GetNextToken();
            auto deferNode = std::make_shared<UnaryNode>();
            deferNode -> Uop = UnaryOperator::Deref;
            deferNode -> Lhs = left;
            auto memberNode = std::make_shared<MemberAccessNode>();
            memberNode -> Lhs = deferNode;
            memberNode -> fieldName = Lex.CurrentToken->Content;
            Lex.ExceptToken(TokenKind::Identifier);
            left = memberNode;
            break;
        }else if(Lex.CurrentToken -> Kind == TokenKind::PPlus){
            Lex.GetNextToken();
            auto addNode = std::make_shared<BinaryNode>();
            addNode -> Lhs = left;
            auto constNode  =  std::make_shared<ConstantNode>(nullptr);
            constNode -> Value = 1;
            constNode ->Type = Type::IntType;
            addNode -> Rhs = constNode;
            addNode -> BinOp = BinaryOperator::Incr;
            left = addNode;
            break;
        }else if(Lex.CurrentToken -> Kind == TokenKind::MMinus){
            Lex.GetNextToken();
            auto addNode = std::make_shared<BinaryNode>();
            addNode -> Lhs = left;
            auto constNode  =  std::make_shared<ConstantNode>(nullptr);
            constNode -> Value = -1;
            constNode ->Type = Type::IntType;
            addNode -> Rhs = constNode;
            addNode -> BinOp = BinaryOperator::Incr;
            left = addNode;
            break;
        }else{
            break;
        }
    }
    return left;
}

std::shared_ptr<Type> Parser::ParseUnionDeclaration() {
    auto unionDeclaration = ParseRecord(RecordType::TagKind::Union);
    for (auto &field : unionDeclaration ->fields) {
        if (unionDeclaration ->Size  < field ->type ->Size){
            unionDeclaration ->Size =  field ->type ->Size;
        }
        if (unionDeclaration ->Align  < field ->type ->Align){
            unionDeclaration ->Align =  field ->type ->Align;
        }
    }
    return unionDeclaration;
}

std::shared_ptr<Type> Parser::ParseStructDeclaration() {
    auto structDeclaration = ParseRecord(RecordType::TagKind::Struct);
    int offset = 0;
    for (auto &field : structDeclaration ->fields) {
        offset = AlignTo(offset,field -> type ->Align);
        field -> Offset = offset;
        offset += field -> type ->Size;
        if (structDeclaration -> Align < field ->type ->Align){
            structDeclaration -> Align = field ->type -> Align;
        }
    }
    structDeclaration -> Size = AlignTo(offset, structDeclaration ->Align);
    return structDeclaration;
}

std::shared_ptr<RecordType> Parser::ParseRecord(RecordType::TagKind recordeType) {
    std::shared_ptr<std::string_view> recordName;
    if (Lex.CurrentToken -> Kind == TokenKind::Identifier){
        recordName = std::make_shared<std::string_view>(Lex.CurrentToken->Content);
        Lex.GetNextToken();
    }
    auto record = std::make_shared<RecordType>();
    record->Kind = recordeType;
    if (Lex.CurrentToken -> Kind ==  TokenKind::LBrace){
        Lex.GetNextToken();
        while(Lex.CurrentToken  -> Kind != TokenKind::RBrace){
            auto type = ParseDeclarationSpec(0);
            std::list<std::shared_ptr<Token>> nameTokens;
            type = ParseDeclarator(type,&nameTokens);
            for(auto &tk:nameTokens){
                record ->fields.push_back(std::make_shared<Field>(type,tk,0));
            }
            Lex.ExceptToken(TokenKind::Semicolon);
        }
        Lex.ExceptToken(TokenKind::RBrace);
    }else {
        if (!recordName){
            DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"except declaration struct/union name!");
        }
       auto type = Scope::GetInstance() -> FindTag(*recordName);
       return std::dynamic_pointer_cast<RecordType>(type);
    }
    if (recordName){
        Scope::GetInstance() -> PushTag(*recordName,record);
    }
    return record;
}

std::shared_ptr<AstNode> Parser::ParseBinaryExpr(int priority) {
    auto leftNode  = ParseCastExpr();
    while(true){
        if (TOpPrecedence[Lex.CurrentToken->Kind] >= priority){
            break;
        }
        switch (Lex.CurrentToken->Kind) {
            case TokenKind::Plus:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Add);
                break;
            case TokenKind::Minus:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Sub);
                break;
            case TokenKind::Asterisk:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Mul);
                break;
            case TokenKind::Slash:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::IDiv);
                break;
            case TokenKind::Assign:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Assign);
                break;
            case TokenKind::Mod:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Mod);
                break;
            case TokenKind::VerticalBar:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Or);
                break;
            case TokenKind::Caret:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Xor);
                break;
            case TokenKind::Sal:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Sal);
                break;
            case TokenKind::Sar:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Sar);
                break;
            case TokenKind::Amp:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::And);
                break;
            case TokenKind::Greater:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Greater);
                break;
            case TokenKind::GreaterEqual:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::GreaterEqual);
                break;
            case TokenKind::Lesser:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Lesser);
                break;
            case TokenKind::LesserEqual:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::LesserEqual);
                break;
            case TokenKind::Equal:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Equal);
                break;
            case TokenKind::NotEqual:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::NotEqual);
                break;

            default:
                return leftNode;
        }
    }
    return leftNode;
}

std::shared_ptr<AstNode> Parser::ParseBinaryOperationExpr(std::shared_ptr<AstNode> left, BinaryOperator op) {
    auto curPriority =  TOpPrecedence[Lex.CurrentToken->Kind];
    Lex.GetNextToken();
    std::shared_ptr<BinaryNode> binaryNode = std::make_shared<BinaryNode>();
    binaryNode ->BinOp = op;
    binaryNode -> Lhs = left;
    binaryNode -> Rhs = ParseBinaryExpr(curPriority);
    return binaryNode;
}

const bool Parser::IsTypeName() {
    if (Lex.CurrentToken -> Kind == TokenKind::Int || Lex.CurrentToken -> Kind == TokenKind::Char
        || Lex.CurrentToken -> Kind == TokenKind::Short || Lex.CurrentToken -> Kind == TokenKind::Long
        || Lex.CurrentToken -> Kind == TokenKind::Float || Lex.CurrentToken -> Kind == TokenKind::Double
        || Lex.CurrentToken -> Kind == TokenKind::Struct || Lex.CurrentToken -> Kind == TokenKind::Union
        || Lex.CurrentToken -> Kind == TokenKind::SIGNED || Lex.CurrentToken -> Kind == TokenKind::UNSIGNED
        || Lex.CurrentToken -> Kind == TokenKind::_Bool){
        return true;
    }
    if(Scope::GetInstance() -> FindTagInCurrentScope(Lex.CurrentToken->Content)){
        return true;
    }
    return false;
}

//ParseCastExpr ::= "(" type-name ")" ParseCastExpr | ParseUnaryExpr
std::shared_ptr<AstNode> Parser::ParseCastExpr() {
    Lex.BeginPeekToken();
    if (Lex.CurrentToken -> Kind == TokenKind::LParent){
        auto castNode = std::make_shared<CastNode>();
        Lex.GetNextToken();
        if (IsTypeName()){
            auto type = ParseDeclarationSpec(0);
            castNode -> Type  = type;
            Lex.SkipToken(TokenKind::RParent);
            castNode -> CstNode = ParseCastExpr();
            return castNode;
        }else{
            Lex.EndPeekToken();
        }
    }
    return ParseUnaryExpr();
}

//ParseDeclarationExpr
std::shared_ptr<AstNode> Parser::ParseDeclarationExpr() {
     if (IsTypeName()){
        std::list<std::shared_ptr<ExprVarNode>> declarationNodes;
        auto tokens = std::list<std::shared_ptr<Token>>();
        auto type = ParseDeclarator(ParseDeclarationSpec(0),&tokens);
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
        //array constant init
        auto valueNode = ParseExpr();
        valueNode-> Type = type;
        for (auto &n:assignNodes)
            n ->Rhs = valueNode;
        multiAssignNode ->AssignNodes = assignNodes;
        return multiAssignNode;
    }
    return nullptr;
}

// "{" (init1,",")* "}"
std::shared_ptr<ConstantNode> Parser::ParseInitListExpr() {
    std::shared_ptr<ConstantNode> cursor = std::make_shared<ConstantNode>(nullptr) ;
    std::shared_ptr<ConstantNode> root = cursor;
    root ->isRoot = true;
    do {
        if (Lex.CurrentToken->Kind == TokenKind::Comma){
            Lex.GetNextToken();
        }
        if (Lex.CurrentToken ->Kind == TokenKind::LBrace){
            Lex.GetNextToken();
            cursor -> Sub = ParseInitListExpr();
        }else{
            cursor->Next  = std::dynamic_pointer_cast<ConstantNode>(ParsePrimaryExpr());
            cursor = cursor -> Next;
        }
    }while(Lex.CurrentToken->Kind == TokenKind::Comma);
    Lex.ExceptToken(TokenKind::RBrace);
    return root;
}


