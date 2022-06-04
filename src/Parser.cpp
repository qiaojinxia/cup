//
// Created by a on 2022/3/8.
//

#include <string>
#include "Parser.h"
#include "Lexer.h"
#include "AstNode.h"
#include "Diag.h"
#include "TypeVisitor.h"
#include "Common.h"

using namespace BDD;


//ParseDeclarationExpr
std::shared_ptr<AstNode> Parser::ParseDeclarationExpr() {
    if(auto emptyNode= ParseEnumDeclaration()){
        return  emptyNode;
    }else if (IsTypeName()){

        std::list<std::shared_ptr<ExprVarNode>> declarationNodes;
        auto tokens = std::list<std::shared_ptr<Token>>();
        std::shared_ptr<Attr> varAttr = std::make_shared<Attr>();
        auto type = ParseDeclarator(ParseDeclarationSpec(varAttr),&tokens);
        for (auto &tk:tokens) {
            auto newVarNode = std::make_shared<ExprVarNode>(tk);
            newVarNode -> Name = tk -> Content;
            newVarNode -> VarObj =  NewLocalVar(newVarNode ->Name, type, varAttr);
            declarationNodes.push_back(newVarNode);
            //store static variable without init value
            if (newVarNode->VarObj->VarAttr->isStatic){
                auto stVarNm = Scope::GetInstance() -> PushStaticVar(newVarNode->Name,type);
                newVarNode ->VarObj->VarAttr->isInit = true;
                newVarNode ->VarObj->GlobalName = stVarNm;
            }
        }
        if (Lex.CurrentToken -> Kind == TokenKind::Semicolon){
            auto multiDeclarationStmtNode = std::make_shared<DeclarationStmtNode>(Lex.CurrentToken);
            multiDeclarationStmtNode -> declarationNodes = declarationNodes;
            multiDeclarationStmtNode ->Type = type;
            return multiDeclarationStmtNode;
        }
        auto multiAssignNode = std::make_shared<DeclarationAssignmentStmtNode>(Lex.CurrentToken);
        std::list<std::shared_ptr<AssignNode>> assignNodes;
        for (auto &leftDec:declarationNodes) {
            auto assignNode = std::make_shared<AssignNode>(Lex.CurrentToken);
            assignNode -> Lhs = leftDec;
            assignNodes.push_back(assignNode);
        }
        Lex.ExceptToken( TokenKind::Assign);
        //array constant init
        auto valueNode = ParseExpr();
        //if func pointer is int (*a)(int,int) = max; not int (*a)(int,int) = &max; Is the same meaning
        // so  we  need to auto convert to &max use unaryNode warp
        auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(valueNode);
        if (type ->IsFuncPointerType() && !unaryNode && valueNode ->Type->IsFunctionType() ){
            auto unaryNode = std::make_shared<UnaryNode>(nullptr);
            unaryNode ->Uop = UnaryOperator::Addr;
            unaryNode ->Lhs = valueNode;
            valueNode = unaryNode;
        }
        valueNode-> Type = type;
        for (auto &n:assignNodes){
            auto leftDec = std::dynamic_pointer_cast<ExprVarNode>(n->Lhs);
            n ->Rhs = valueNode;
            //static var init value
            if (leftDec->VarObj->VarAttr->isStatic){
                auto cstNode = std::dynamic_pointer_cast<ConstantNode>(valueNode);
                TypeVisitor typeVisitor;
                n  ->Accept(&typeVisitor);
                Scope::GetInstance() -> PushStaticVar(leftDec->VarObj->GlobalName, cstNode);
            }else{
                multiAssignNode ->AssignNodes.push_back(n);
            }
        }
        return multiAssignNode;
    }
    return nullptr;
}


// primary = "(" ParseCompoundStmt ")"
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
    std::shared_ptr<AstNode> node ;
    switch (Lex.CurrentToken -> Kind){
        case TokenKind::LParent:
        {
            Lex.GetNextToken();
            if (Lex.CurrentToken -> Kind == TokenKind::LBrace){
                node =  ParseCompoundStmt();
                break;
            }
            node = ParseExpr();
            Lex.ExceptToken(TokenKind::RParent);
            break;
        }
        case TokenKind::LBrace:
        {
            node =  ParseCompoundStmt();
            break;
        }
        case TokenKind::Identifier:
        {
            Lex.BeginPeekToken();
            Lex.GetNextToken();
            if (Lex.CurrentToken -> Kind == TokenKind::LParent){
                Lex.EndPeekToken();
                auto funcCallNode =  ParseFuncCallNode();
                //to record funcall node if return is structType to distribution  memory in stack
                if (funcCallNode ->Type->GetBaseType()->IsStructType())
                    CurFuncCall.push_back(std::dynamic_pointer_cast<FuncCallNode>(funcCallNode));
                return  funcCallNode;
            }
            Lex.EndPeekToken();

            auto exprVarNode = std::make_shared<ExprVarNode>(Lex.CurrentToken);
            exprVarNode -> Name = Lex.CurrentToken->Content;
            auto nodeName = std::string (Lex.CurrentToken -> Content);
            auto varObj = FindLocalVar(Lex.CurrentToken -> Content);
            if (!varObj){
                auto constObj = Scope::GetInstance()->GetConstantTable()[nodeName];
                if (!constObj){
                    auto funcSign = Scope::GetInstance()->GetFuncSign(Lex.CurrentToken->Content);
                    if (!funcSign){
                        DiagLoc(Lex.SourceCode, Lex.GetLocation(), "undefined variable|enum %s ", nodeName.data());
                    }
                    exprVarNode ->Type = funcSign ->FuncType;
                    node = exprVarNode;
                }else{
                    node = constObj;
                }
            }else{
                exprVarNode -> Type = varObj-> Type;
                exprVarNode ->VarObj = varObj;
                node = exprVarNode;
            }
            Lex.GetNextToken();
            break;
        }
        case  TokenKind::CharNum:
        {
            auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            constNode -> Value = Lex.CurrentToken -> Value;
            constNode -> Type = Type::CharType;
            Lex.GetNextToken();
            constNode -> isChar = true;
            node =  constNode;
            break;
        }
       case TokenKind::Num :
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
            constNode -> Type = Type::PtrCharType;
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
            auto sizeOfNode = std::make_shared<SizeOfExprNode>(Lex.CurrentToken);
            Lex.GetNextToken();
            if (Lex.CurrentToken ->Kind == TokenKind::LParent){
                if (IsTypeName()){
                    Lex.GetNextToken();
                    std::shared_ptr<Attr> varAttr = std::make_shared<Attr>();
                    auto type = ParseDeclarationSpec(varAttr);
                    auto emptyNode= std::make_shared<EmptyNode>(nullptr);
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
            DiagLoc(Lex.SourceCode,Lex.GetLocation(),"snot support type",Lex.CurrentToken->Kind);
    }
    return node;
}

//ParseExpr ::= ParseBinaryExpr | ParseTernaryExpr ｜ ParseDeclarationExpr
std::shared_ptr<AstNode> Parser::ParseExpr() {
    auto declarationNode = ParseDeclarationExpr();
    if (declarationNode){
        return declarationNode;
    }
    auto binaryNode = ParseBinaryExpr(16);
    if (Lex.CurrentToken ->Kind == TokenKind::Semicolon ){
        return binaryNode;
    }
    if (Lex.CurrentToken ->Kind == TokenKind::QuestionMark){
        auto ternaryNode = ParseTernaryExpr(binaryNode);
        return ternaryNode;
    }
    return binaryNode;
}

std::shared_ptr<ProgramNode> Parser::Parse() {
    Scope::GetInstance() -> PushScope("p");
    auto node = std::make_shared<ProgramNode>(Lex.CurrentToken);
    while (Lex.CurrentToken -> Kind != TokenKind::Eof){
        if (ParseExtern()){
            continue;
        }
        if(ParseTypeDef()){
            continue;
        }
        node ->Funcs.push_back(ParseFunc());
    }
    Scope::GetInstance() -> PopScope();
    return node;
}

std::shared_ptr<AstNode> Parser::ParseStatement() {
    switch (Lex.CurrentToken -> Kind) {
        case TokenKind::If:
            return ParseIfElseStmt();
        case TokenKind::Switch:
            return ParseSwitchCaseStmt();
        case TokenKind::LBrace:
            return ParseBlock();
        case TokenKind::While:
            return ParseWhileStmt();
        case TokenKind::Do:
            return ParseDoWhileStmt();
        case TokenKind::For:
            return ParseForStmt();
        case TokenKind::Return:
            return ParseReturnStmt();
        case TokenKind::Break:
            return ParseBreakStmt();
        case TokenKind::Continue:
            return ParseContinueStmt();
        case TokenKind::TypeDef:
            return ParseTypeDef();
        default:
        {
            auto exprNode = std::make_shared<ExprStmtNode>(Lex.CurrentToken);
            if (Lex.CurrentToken -> Kind != TokenKind::Semicolon)
                exprNode -> Lhs = ParseExpr();
            Lex.ExceptToken(TokenKind::Semicolon);
            return exprNode;
        }
    }
}

std::shared_ptr<Var> Parser::FindLocalVar(std::string_view varName) {
    return Scope::GetInstance() -> FindVar(varName);
}

std::shared_ptr<Var> Parser::NewLocalVar(std::string_view varName,std::shared_ptr<Type> type,std::shared_ptr<Attr> attr) {
    auto obj = std::make_shared<Var>();
    obj -> Type = type;
    obj ->Name = varName;
    obj -> Offset = 0;
    obj -> VarAttr  = attr;
    LocalVars -> push_back(obj);
    Scope::GetInstance() -> PushVar(obj);
    return obj;
}

std::shared_ptr<ExprVarNode> Parser::GetVarExprNode(std::shared_ptr<AstNode> node) {
    auto exprVarNode = std::dynamic_pointer_cast<ExprVarNode>(node);
    return exprVarNode;
}


std::shared_ptr<AstNode> Parser::ParseFunc() {
    auto node = std::make_shared<FunctionNode>(Lex.CurrentToken);
    LocalVars = &node -> Locals;
    std::shared_ptr<Attr> varAttr = std::make_shared<Attr>();
    auto type = ParseDeclarationSpec(varAttr);
    std::list<std::shared_ptr<Token>> nameTokens;
    auto FuncNameToken = Lex.CurrentToken;
    node -> FuncName = FuncNameToken->Content;
    Scope::GetInstance() -> PushScope(node -> FuncName);
    type = ParseDeclarator(type,&nameTokens);
    node -> Type = type;
    std::shared_ptr<FunctionType> funcType = std::dynamic_pointer_cast<FunctionType>(type);
    if (funcType != nullptr){
        for(auto it = funcType -> Params.rbegin();it != funcType -> Params.rend();++it) {
            //if paramer is arry actually is pointer
            if ((*it)->Type->IsArrayType()) {
                auto  pType = std::make_shared<PointerType>((*it)->Type);
                node->Params.push_front(NewLocalVar((*it)->TToken->Content, pType,(*it)->ParamAttr));
            } else {
                node->Params.push_front(NewLocalVar((*it)->TToken->Content, (*it)->Type, (*it)->ParamAttr));
            }
        }
    }
    TypeVisitor typeVisitor;

    Lex.ExceptToken(TokenKind::LBrace);

    while (Lex.CurrentToken -> Kind != TokenKind::RBrace){
        auto stmtNode = ParseStatement();
        node -> Stmts.push_back(stmtNode);

        // when getUser return a struct write to caller stack we need allocate memory give Callee to write return data
        //if funCall return struct but is DeclarationAssignmentStmtNode need't  allocate additional space direct store to  variable
        // such as struct user a = getUser(); in others, we need allocate memory space to temporary storage to copy to other variables
        // such as struct user a; a = getUser();
        auto exptStmt = std::dynamic_pointer_cast<ExprStmtNode>(stmtNode);
        bool isDecr;
        if (exptStmt){
            auto declarationStmtNode = std::dynamic_pointer_cast<DeclarationAssignmentStmtNode>(exptStmt->Lhs);
            declarationStmtNode ? isDecr = true : isDecr = false;
        }
        while (!CurFuncCall.empty()){
            if (isDecr){
                CurFuncCall.pop_front();
                isDecr = false;
                continue;
            }
            node->InnerFunCallStmts.push_back(CurFuncCall.front());
            CurFuncCall.pop_front();
        }
        //check return type
        if(CurFuncReturnNode.empty())
            continue;
        for (auto &rtStmt:CurFuncReturnNode) {
            // if return a struct that store address (ReturnOffset(%%rbp)) the data need to copy to
            if (node->Type->GetBaseType()->IsStructType()){
                rtStmt ->ReturnOffset = -1 * Type::VoidType->Size;
            }
            node -> ReturnStmts.push_back(rtStmt);
            if (rtStmt -> ReturnVarExpr)
                node ->ReturnVarMap[rtStmt -> ReturnVarExpr ->Name] = rtStmt -> ReturnVarExpr;
            rtStmt -> Type = type->GetBaseType();
            rtStmt ->Accept(&typeVisitor);
            if (!Type::IsTypeEqual(type->GetBaseType(),rtStmt->Type)){
                auto tips =  string_format("excepted return type %s  get type %s !",type->GetBaseType()->Align,rtStmt->Type->Align);
                DiagLoc(Lex.SourceCode, rtStmt->Tk->Location,tips.c_str());
            }
        }
        CurFuncReturnNode.clear();
    }

    //if func return type is struct and just  one var return in func  direct set var address pointer to caller stack
    //such as User getUser(){ User a ={xxx,xxx,xxx}; return a;} void main(){ getUser();} In this case, set a pointer to caller stack to write struct
    if (node->ReturnVarMap.size() == 1){
        auto varExprNode = node->ReturnStmts.front()->ReturnVarExpr;
        varExprNode->VarObj->Offset = Type::Pointer->Size * -1;
        varExprNode ->VarObj-> VarAttr -> isInit = true;
        varExprNode ->VarObj-> VarAttr -> isReference = true;
    }

    auto funcSign = std::make_shared<FuncSign>(std::dynamic_pointer_cast<FunctionType>(type));
    funcSign ->FuncName = node ->FuncName;

    if (Scope::GetInstance() ->GetFuncSign(funcSign ->FuncName)){
        auto tips =  string_format("redefinition of '%s' !",std::string(funcSign ->FuncName).c_str());
        DiagLoc(Lex.SourceCode, FuncNameToken->Location,tips.c_str());
    }


    if (node->ReturnStmts.empty() && node->Type->GetBaseType() != Type::VoidType)
        DiagLoc(Lex.SourceCode,FuncNameToken->Location, string_format("func %s excepted return ",std::string(funcSign ->FuncName).data()).data());

    Scope::GetInstance() ->PushFuncSign(funcSign);

    Scope::GetInstance() -> PopScope();
    Lex.ExceptToken(TokenKind::RBrace);
    return node;
}
//ParseFuncCallNode ::=
std::shared_ptr<AstNode> Parser::ParseFuncCallNode() {
    auto node = std::make_shared<FuncCallNode>(Lex.CurrentToken);
    node -> FuncName = Lex.CurrentToken -> Content;
    std::shared_ptr<FunctionType> funcType;
    //check func already declared
    auto funcSign = Scope::GetInstance() ->GetFuncSign(node -> FuncName);
    //if can't find func in declar table next find func pointer table
    if (!funcSign){
        auto  varNode = Scope::GetInstance() ->FindVar(Lex.CurrentToken->Content);
        if (!varNode || !varNode ->Type->IsFuncPointerType()){
            DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"func must declare first !");
        }
        auto varExprNode =std::make_shared<ExprVarNode>(nullptr);
        varExprNode -> VarObj = varNode;
        node ->FuncPointerOffset = varExprNode;
        auto varNodeType = varNode->Type;
        if (varNode ->Type->IsAliasType())
            varNodeType = varNodeType->GetBaseType();
        funcType = std::dynamic_pointer_cast<FunctionType>(varNodeType ->GetBaseType());
    }else{
        funcType = funcSign ->FuncType;
    }
    Lex.ExceptToken(TokenKind::Identifier);
    Lex.ExceptToken(TokenKind::LParent);
    int i = 0;
    if (funcType->IsFunctionType() && funcType->GetBaseType()->IsStructType())
        i = 1; //if return struct first args is return struct write to address
    while(Lex.CurrentToken->Kind != TokenKind::RParent){
        if (Lex.CurrentToken ->Kind == TokenKind::Comma){
            Lex.GetNextToken();
        }
        auto argNType = funcType ->Params[i]->Type;
        TypeVisitor typeVisitor;
        auto arg = ParseExpr();
        typeVisitor.CurAssignType = argNType;
        arg ->Accept(&typeVisitor);

        if (!Type::IsTypeEqual(argNType, arg->Type)){
            auto tips =  string_format("Input type of parameter is incorrect. It should be %s "
                                           "instead of %s. Please check!", argNType->Alias, arg->Type->Alias);
            DiagLoc(Lex.SourceCode, arg->Tk->Location,tips.c_str());
        }
        node -> Args.push_back(arg);
    }

    node -> Type = funcType;
    //Check Func args
    Lex.ExceptToken(TokenKind::RParent);
    return node;
}

//ParseDeclarationSpec :=  ( int ｜ char | short | long | union | struct | union | float | double | const | * | static )  ParseDeclarationSpec |  ε
std::shared_ptr<Type> Parser::ParseDeclarationSpec(std::shared_ptr<Attr> attr) {
    bool isConstant = false;
    int baseType = 0;
    std::shared_ptr<Type> sType;
    std::shared_ptr<Type> type;
    while(true){
        if (Lex.CurrentToken -> Kind == TokenKind::Const){
            isConstant = true;
            Lex.GetNextToken();
            continue;
        }else if (Lex.CurrentToken -> Kind == TokenKind::Static){
            if (!attr)
                DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"invalid keyword !");
            attr->isStatic = true;
            Lex.GetNextToken();
            continue;
        }else if (Lex.CurrentToken -> Kind == TokenKind::Void){
            Lex.GetNextToken();
            baseType = (int) BuildInType::Kind::Void;
            continue;
        }else if (Lex.CurrentToken -> Kind == TokenKind::Int){
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
            if (sType){
                if (isConstant){
                    sType = std::make_shared<AliasType>(sType,Lex.CurrentToken);
                    sType->constant = true;
                }
                sType = std::make_shared<PointerType>(sType);
            }else{
                sType = std::make_shared<PointerType>(GenerateType(baseType,isConstant));
            }
            isConstant = false;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::SIGNED){
            Lex.GetNextToken();
            baseType |= (int) BuildInType::Kind::Signed;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::UNSIGNED){
            Lex.GetNextToken();
            baseType |= (int) BuildInType::Kind::UnSigned;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Enum){
            Lex.GetNextToken();
            auto enumType = std::make_shared<EnumType>(Lex.CurrentToken);
            return enumType;
        }else{
            type = Scope::GetInstance() -> FindTag(Lex.CurrentToken ->Content);
            if (type){
                if (isConstant){
                    type = std::make_shared<AliasType>(type,Lex.CurrentToken);
                    type -> constant = true;
                }
                isConstant = false;
                sType = type;
                Lex.GetNextToken();
                continue;
            }
            break;
        }
    }
    if (sType){
        if (isConstant) {
            sType = std::make_shared<AliasType>(sType, Lex.CurrentToken);
            sType -> constant = true;
        }
        return sType;
    }
    type = GenerateType(baseType,isConstant);
    return type;
}

std::shared_ptr<Type> Parser::GenerateType(int baseType,bool isConstant) const {
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
    if (isConstant){
        type = std::make_shared<AliasType>(type,Lex.CurrentToken);
        type -> constant = true;
    }
    return type;
}
//ParseTypeSuffix ::=  "(" (ParseDeclarationSpec,ParseDeclarator)? ")" |  ("[" ParseTypeSuffix "]")* | ε
std::shared_ptr<Type> Parser::ParseTypeSuffix(std::shared_ptr<Type> baseType) {
    std::shared_ptr<Token> FuncPointerName;
    if (Lex.CurrentToken -> Kind == TokenKind::LParent){
        Lex.BeginPeekToken();
        Lex.GetNextToken();
        if (Lex.CurrentToken -> Kind == TokenKind::Asterisk){
            Lex.GetNextToken();
            FuncPointerName = Lex.CurrentToken;
            Lex.GetNextToken();
            Lex.ExceptToken(TokenKind::RParent);
        }else{
            Lex.EndPeekToken();
        }
        auto funcType = std::make_shared<FunctionType>(baseType);
        //if return type is struct  set the first param is  struct offset
        if(funcType->GetBaseType()->IsStructType()){
            auto param = std::make_shared<Param>();
            param -> Type = std::make_shared<RecordType>();
            param ->TToken = std::make_shared<Token>();
            param ->TToken ->Content = "r_st";
            std::shared_ptr<Attr> paramAttr = std::make_shared<Attr>();
            param -> ParamAttr = paramAttr;
            funcType -> Params.push_back(param);
        }
        Lex.GetNextToken();
        std::list<std::shared_ptr<Token>> tokens;
        while (Lex.CurrentToken -> Kind != TokenKind::RParent){
                std::shared_ptr<Attr> paramAttr = std::make_shared<Attr>();
                auto type = ParseDeclarator(ParseDeclarationSpec(paramAttr),&tokens);
                if(paramAttr ->isStatic)
                    DiagLoc(Lex.SourceCode,Lex.GetLocation(),"invalid storage class specifier in function declarator");
                auto param = std::make_shared<Param>();
                param ->Type = type;
                param ->ParamAttr = paramAttr;
                if (!tokens.empty())
                    param ->TToken = tokens.back();
                funcType ->  Params.push_back(param);
        }
        Lex.ExceptToken(TokenKind::RParent);
        if (FuncPointerName){
            auto funcPointerType = std::make_shared<PointerType>(funcType);
            return funcPointerType;
        }
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
    if (Lex.CurrentToken->Kind == TokenKind::Comma){
        Lex.GetNextToken();
        return type;
    }
    if (Lex.CurrentToken->Kind == TokenKind::Identifier){
        while(Lex.CurrentToken -> Kind == TokenKind::Identifier && !IsTypeName() ){
            if (nameTokens){
                (*nameTokens).push_back(Lex.CurrentToken);
            }
            Lex.SkipToken(TokenKind::Comma);
        }
    }else if (Lex.CurrentToken ->Kind == TokenKind::LParent){
        Lex.BeginPeekToken();
        Lex.GetNextToken();
        if (Lex.CurrentToken ->Kind != TokenKind::RParent){
            Lex.GetNextToken();
            if(Lex.CurrentToken ->Kind == TokenKind::Identifier){
                (*nameTokens).push_back(Lex.CurrentToken);
            }else{
                DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"except func pointer name!");
            }
        }
        Lex.EndPeekToken();
    }else if (Lex.CurrentToken ->Kind == TokenKind::RParent){
        return type;
    }else{
        DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"except variable name!");
    }
    return ParseTypeSuffix(type);
}

//ParseUnaryExpr ::= (+ | - | * | & | ～ ｜ ! )? ParseCastExpr | ParsePostFixExpr
std::shared_ptr<AstNode> Parser::ParseUnaryExpr() {
    auto node = std::make_shared<UnaryNode>(Lex.CurrentToken);
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
        case TokenKind::Tilde:
                node -> Uop = UnaryOperator::BitNot;
            break;
        case TokenKind::PPlus:
            node -> Uop = UnaryOperator::Incr;
            break;
        case TokenKind::MMinus:
            node -> Uop = UnaryOperator::Decr;
            break;
        case TokenKind::ExclamationMark:
            node -> Uop = UnaryOperator::Not;
            break;
        default:
            return ParsePostFixExpr();
    }
    Lex.GetNextToken();
    node -> Lhs = ParseCastExpr();
    return node;
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
            auto starNode = std::make_shared<ArrayMemberNode>(Lex.CurrentToken);
            Lex.GetNextToken();
            starNode -> Offset = ParseExpr();
            starNode -> Lhs = left;
            Lex.ExceptToken(TokenKind::RBracket);
            left = starNode;
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::Period){
            auto memberNode = std::make_shared<MemberAccessNode>(Lex.CurrentToken);
            Lex.GetNextToken();
            memberNode -> Lhs = left;
            memberNode -> fieldName = Lex.CurrentToken -> Content;
            left = memberNode;
            Lex.ExceptToken(TokenKind::Identifier);
            continue;
        }else if(Lex.CurrentToken -> Kind == TokenKind::PointerTo){
            auto deferNode = std::make_shared<UnaryNode>(Lex.CurrentToken);
            Lex.GetNextToken();
            deferNode -> Uop = UnaryOperator::Deref;
            deferNode -> Lhs = left;
            auto memberNode = std::make_shared<MemberAccessNode>(Lex.CurrentToken);
            memberNode -> Lhs = deferNode;
            memberNode -> fieldName = Lex.CurrentToken->Content;
            Lex.ExceptToken(TokenKind::Identifier);
            left = memberNode;
            break;
        }else if(Lex.CurrentToken -> Kind == TokenKind::PPlus){
            auto incrNode = std::make_shared<IncrNode>(Lex.CurrentToken);
            Lex.GetNextToken();
            if (left ->Type->IsConstant()){
                DiagLoc(Lex.SourceCode, Lex.GetLocation(), "constant can't change!");
            }
            incrNode -> Lhs = left;
            auto constNode  =  std::make_shared<ConstantNode>(nullptr);
            constNode -> Value = 1;
            constNode ->Type = Type::IntType;
            incrNode -> Rhs = constNode;
            incrNode -> BinOp = BinaryOperator::Incr;
            left = incrNode;
            break;
        }else if(Lex.CurrentToken -> Kind == TokenKind::MMinus){
            auto decrNode = std::make_shared<DecrNode>(Lex.CurrentToken);
            Lex.GetNextToken();
            decrNode -> Lhs = left;
            auto constNode  =  std::make_shared<ConstantNode>(nullptr);
            constNode -> Value = -1;
            constNode ->Type = Type::IntType;
            decrNode -> Rhs = constNode;
            decrNode -> BinOp = BinaryOperator::Decr;
            left = decrNode;
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
            auto type = ParseDeclarationSpec(nullptr);
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
    TypeVisitor typeVisitor;
    auto leftNode  = ParseCastExpr();
    while(true){
        if (TopPrecedence[Lex.CurrentToken->Kind] >= priority){
            break;
        }
        switch (Lex.CurrentToken->Kind) {
            case TokenKind::Plus:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Add);
                break;
            case TokenKind::Minus:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Minus);
                break;
            case TokenKind::Asterisk:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Mul);
                break;
            case TokenKind::Slash:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::IDiv);
                break;
            case TokenKind::Assign:
                leftNode->Accept(&typeVisitor);
                if (leftNode ->Type->IsConstant()){
                    DiagLoc(Lex.SourceCode, leftNode->Tk->Location, "constant  can't change!");
                }
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Assign);
                break;
            case TokenKind::Mod:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Mod);
                break;
            case TokenKind::VerticalBar:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitOr);
                break;
            case TokenKind::Caret:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitXor);
                break;
            case TokenKind::Sal:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitSal);
                break;
            case TokenKind::Sar:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitSar);
                break;
            case TokenKind::Amp:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitAnd);
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
            case TokenKind::And:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::And);
                break;
            case TokenKind::Or:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Or);
                break;
            default:
                return leftNode;
        }
    }
    return leftNode;
}

std::shared_ptr<AstNode> Parser::ParseBinaryOperationExpr(std::shared_ptr<AstNode> left, BinaryOperator op) {
    auto curPriority =  TopPrecedence[Lex.CurrentToken->Kind];
    Lex.GetNextToken();
    std::shared_ptr<BinaryNode> binaryNode;
    switch (op){
        case BinaryOperator::Assign:
        {
            auto assignNode = std::make_shared<AssignNode>(Lex.CurrentToken);
            assignNode -> Lhs = left;
            assignNode -> Rhs = ParseBinaryExpr(curPriority);
            //if assign rhs is func convert to &func
            if (auto exprNode = std::dynamic_pointer_cast<ExprVarNode>(assignNode ->Rhs)){
                if(Scope::GetInstance()->GetFuncSign(exprNode->Name)){
                    auto  unaryNode = std::make_shared<UnaryNode>(exprNode->Tk);
                    unaryNode ->Lhs = assignNode->Rhs;
                    unaryNode->Uop = UnaryOperator::Addr;
                    assignNode ->Rhs = unaryNode;
                }
            }
            binaryNode = assignNode;
            break;
        }
        case BinaryOperator::Add:
        {
            auto addNode = std::make_shared<AddNode>(Lex.CurrentToken);
            addNode -> Lhs = left;
            addNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = addNode;
            break;

        }
        case BinaryOperator::Minus:
        {
            auto minusNode = std::make_shared<MinusNode>(Lex.CurrentToken);
            minusNode -> Lhs = left;
            minusNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = minusNode;
            break;

        }
        case BinaryOperator::Mul:
        {
            auto mulNode = std::make_shared<MulNode>(Lex.CurrentToken);
            mulNode -> Lhs = left;
            mulNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = mulNode;
            break;

        }
        case BinaryOperator::IDiv:
        {
            auto divNode = std::make_shared<DivNode>(Lex.CurrentToken);
            divNode -> Lhs = left;
            divNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = divNode;
            break;

        }
        case BinaryOperator::Mod:
        {
            auto modNode = std::make_shared<ModNode>(Lex.CurrentToken);
            modNode -> Lhs = left;
            modNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = modNode;
            break;

        }
        case BinaryOperator::Lesser:
        case BinaryOperator::LesserEqual:
        case BinaryOperator::Greater:
        case BinaryOperator::GreaterEqual:
        case BinaryOperator::Equal:
        case BinaryOperator::NotEqual:
        {
            auto cmpNode = std::make_shared<CmpNode>(Lex.CurrentToken);
            cmpNode -> Lhs = left;
            cmpNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = cmpNode;
            break;
        }
        case BinaryOperator::BitAnd:
        case BinaryOperator::BitOr:
        case BinaryOperator::BitXor:
        case BinaryOperator::BitSal:
        case BinaryOperator::BitSar:
        {
            auto bitOpNode = std::make_shared<BitOpNode>(Lex.CurrentToken);
            bitOpNode -> Lhs = left;
            bitOpNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = bitOpNode;
            break;
        }
        case BinaryOperator::And:
        {
            auto andNode = std::make_shared<AndNode>(Lex.CurrentToken);
            andNode -> Lhs = left;
            andNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = andNode;
            break;
        }
        case BinaryOperator::Or:
        {
            auto orNode = std::make_shared<OrNode>(Lex.CurrentToken);
            orNode -> Lhs = left;
            orNode -> Rhs = ParseBinaryExpr(curPriority);
            binaryNode = orNode;
            break;
            break;
        }
        default:
            assert(0);
    }
    binaryNode ->BinOp = op;
    return binaryNode;
}

bool Parser::IsTypeName() {
    if (Lex.CurrentToken -> Kind == TokenKind::Int || Lex.CurrentToken -> Kind == TokenKind::Char
        || Lex.CurrentToken -> Kind == TokenKind::Short  || Lex.CurrentToken -> Kind == TokenKind::Long
        || Lex.CurrentToken -> Kind == TokenKind::Float  || Lex.CurrentToken -> Kind == TokenKind::Double
        || Lex.CurrentToken -> Kind == TokenKind::Struct || Lex.CurrentToken -> Kind == TokenKind::Union
        || Lex.CurrentToken -> Kind == TokenKind::SIGNED || Lex.CurrentToken -> Kind == TokenKind::UNSIGNED
        || Lex.CurrentToken -> Kind == TokenKind::_Bool  || Lex.CurrentToken -> Kind == TokenKind::Enum
        || Lex.CurrentToken -> Kind == TokenKind::Const  || Lex.CurrentToken -> Kind == TokenKind::Static
        || Lex.CurrentToken -> Kind == TokenKind::Void){
        return true;
    }
    if(Scope::GetInstance() -> FindTag(Lex.CurrentToken->Content)){
        return true;
    }
    return false;
}

//ParseCastExpr ::= "(" type-name ")" ParseCastExpr | ParseUnaryExpr
std::shared_ptr<AstNode> Parser::ParseCastExpr() {
    Lex.BeginPeekToken();
    if (Lex.CurrentToken -> Kind == TokenKind::LParent){
        auto castNode = std::make_shared<CastNode>(Lex.CurrentToken);
        Lex.GetNextToken();
        if (IsTypeName()){
            auto type = ParseDeclarationSpec(nullptr);
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


std::shared_ptr<ConstantNode> Parser::parseInitListExpr(std::shared_ptr<ConstantNode> root) {
    if(!root){
        root = std::make_shared<ConstantNode>(nullptr);
        root ->isRoot = true;
    }
    std::shared_ptr<ConstantNode> cursor= root;
    do {
        if (Lex.CurrentToken->Kind == TokenKind::Comma){
            Lex.GetNextToken();
        }
        if (Lex.CurrentToken ->Kind == TokenKind::LBrace){
            Lex.GetNextToken();
            std::shared_ptr<ConstantNode> nextNode = std::make_shared<ConstantNode>(nullptr);
            nextNode ->isRoot = true;
            parseInitListExpr(nextNode);
            cursor -> Next = nextNode;
            nextNode ->Sub = nextNode->Next;
            Lex.ExceptToken(TokenKind::RBrace);
        }else {
            cursor->Next  = std::dynamic_pointer_cast<ConstantNode>(ParsePrimaryExpr());
            if (!cursor ->Next)
                return nullptr;
        }
        cursor = cursor -> Next;
    }while(Lex.CurrentToken->Kind == TokenKind::Comma);
    return root;
}

// ParseInitListExpr ::= "{" (ParsePrimaryExpr,",")* | (ParsePrimaryExpr:ParsePrimaryExpr ",")* "}"
std::shared_ptr<ConstantNode> Parser::ParseInitListExpr() {
    Lex.BeginPeekToken();
    if (Lex.CurrentToken -> Kind == TokenKind::LBrace){
        Lex.ExceptToken(TokenKind::LBrace);
        auto initCstNode = parseInitListExpr(nullptr);
        Lex.ExceptToken(TokenKind::RBrace);
        if(initCstNode ->HasSetValue()){
            return initCstNode;
        }
        Lex.EndPeekToken();
    }
    return nullptr;
}

//exampleCode ` enum CaoMao { Caomao1 = 1, Caomao2 = 2, Caomao3 }`
// ParseEnumDeclaration ::= "enum" TgName "{" ( enumName ( "=" value )* "," )+ "}"
std::shared_ptr<AstNode> Parser::ParseEnumDeclaration() {
    if (Lex.CurrentToken ->Kind != TokenKind::Enum){
        return nullptr;
    }
    Lex.ExceptedNextToken(TokenKind::Identifier);
    auto enumType = std::make_shared<EnumType>(Lex.CurrentToken);
    Lex.ExceptedNextToken(TokenKind::LBrace);
    Lex.GetNextToken();
    int count = 0;
    while(Lex.CurrentToken ->Kind == TokenKind::Identifier){
        auto enumName = Lex.CurrentToken;
        std::shared_ptr<ConstantNode> constNode;
        Lex.GetNextToken();
        if (Lex.CurrentToken ->Kind == TokenKind::Assign){
            Lex.GetNextToken();
            constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            count = 0;
            Lex.GetNextToken();
        }else{
            constNode = std::make_shared<ConstantNode>(nullptr);
            constNode ->Value = count;
            count += 1;
        }
        constNode ->Name = std::string(enumName->Content);
        constNode ->Type = enumType;
        Scope::GetInstance() ->PutToConstantTable(constNode);
        if (Lex.CurrentToken ->Kind == TokenKind::Comma){
            Lex.GetNextToken();
        }else{
            break;
        }
    }
    Scope::GetInstance() ->PushTag(enumType->token->Content,enumType);
    Lex.ExceptToken(TokenKind::RBrace);
    return std::make_shared<EmptyNode>(Lex.CurrentToken);
}

//exampleCode  `a < 7 ? a = 1 : a = 0;`
//ParseTernaryExpr ::= condition "?" ParseExpr ":" ParseExpr;
std::shared_ptr<AstNode> Parser::ParseTernaryExpr(std::shared_ptr<AstNode> condition) {
    auto ternaryNode = std::make_shared<TernaryNode>(Lex.CurrentToken);
    ternaryNode -> Cond = condition;
    Lex.ExceptToken(TokenKind::QuestionMark);
    ternaryNode->Then = ParseExpr();
    Lex.ExceptToken(TokenKind::Colon);
    ternaryNode->Else = ParseExpr();
    return ternaryNode;
}

//example switch (7) { case 7: xxxx; break; case 6: xxxx;break;}
//ParseSwitchCaseStmt ::= "switch" "(" num ")" "{" (("case" | "default") ":" )+ statements  （"break")?  +  "}"
std::shared_ptr<AstNode> Parser::ParseSwitchCaseStmt() {
    bool isEmpty = true;
    auto switchCaseNode = std::make_shared<SwitchCaseSmtNode>(Lex.CurrentToken);
    Lex.ExceptToken(TokenKind::Switch);
    Lex.ExceptToken(TokenKind::LParent);
    switchCaseNode ->Value = ParseCastExpr();
    Lex.ExceptToken(TokenKind::RParent);
    Lex.ExceptToken(TokenKind::LBrace);
    auto branchMap = std::map<std::shared_ptr<AstNode>,std::list<std::shared_ptr<AstNode>>>();
    while (Lex.CurrentToken->Kind == TokenKind::Case || Lex.CurrentToken->Kind == TokenKind::Default ){
        auto statementsNode = std::list<std::shared_ptr<AstNode>>();
        std::shared_ptr<AstNode> branchCond;
        if (Lex.CurrentToken->Kind ==TokenKind::Case){
            Lex.GetNextToken();
            branchCond = ParseCastExpr();
        }else if(Lex.CurrentToken->Kind ==TokenKind::Default){
            Lex.GetNextToken();
        }
        Lex.ExceptToken(TokenKind::Colon);
        while(Lex.CurrentToken->Kind != TokenKind::Case && Lex.CurrentToken->Kind != TokenKind::Default
        &&  Lex.CurrentToken->Kind != TokenKind::RBrace){
            statementsNode.push_back(ParseStatement());
        }
        if (!statementsNode.empty() && isEmpty)
            isEmpty = false;
        if(branchCond){
            branchMap[branchCond] = statementsNode;
        }else{
            switchCaseNode ->DefaultBranch = statementsNode;
        }
    }
    if (isEmpty){
        auto emptyNode = std::make_shared<EmptyNode>(Lex.CurrentToken);
        return emptyNode;
    }
    switchCaseNode ->CaseBranch = branchMap;
    Lex.ExceptToken(TokenKind::RBrace);
    return switchCaseNode;
}

//exampleCode: if( a < num ){}else{}
//ParseIfElseStmt ::= "if" "(" ( ParseExpr() ("||","&&")* )+ ")"
std::shared_ptr<AstNode> Parser::ParseIfElseStmt() {
    auto node = std::make_shared<IfElseStmtNode>(Lex.CurrentToken);
    Lex.ExceptToken(TokenKind::If);
    Lex.ExceptToken(TokenKind::LParent);
    node ->Cond = ParseExpr();
    Lex.ExceptToken(TokenKind::RParent);
    if (Lex.CurrentToken ->Kind != TokenKind::RBrace){
        node -> Then = ParseBlock();
    }
    if (Lex.CurrentToken -> Kind == TokenKind::Else){
        Lex.GetNextToken();
        if (Lex.CurrentToken -> Kind != TokenKind::RBrace){
            node -> Else = ParseBlock();
        }
    }
    if (node ->Then == nullptr && node ->Else == nullptr){
        return std::make_shared<EmptyNode>(Lex.CurrentToken);
    }
    return node;
}

std::shared_ptr<AstNode> Parser::ParseDoWhileStmt() {
    auto doWhileNode = std::make_shared<DoWhileStmtNode>(Lex.CurrentToken);
    Lex.GetNextToken();
    doWhileNode -> Stmt = ParseStatement();
    Lex.ExceptToken(TokenKind::While);
    Lex.ExceptToken(TokenKind::LParent);
    doWhileNode -> Cond = ParseExpr();
    Lex.ExceptToken(TokenKind::RParent);
    return doWhileNode;
}

std::shared_ptr<AstNode> Parser::ParseForStmt() {
    auto forNode = std::make_shared<ForStmtNode>(Lex.CurrentToken);
    Lex.ExceptToken(TokenKind::For);
    Lex.ExceptToken(TokenKind::LParent);
    if (Lex.CurrentToken->Kind != TokenKind::Semicolon){
        forNode -> Init = ParseExpr();
        Lex.ExceptToken(TokenKind::Semicolon);
        if (Lex.CurrentToken -> Kind !=TokenKind::Semicolon)
            forNode -> Cond = ParseExpr();
        Lex.ExceptToken(TokenKind::Semicolon);
        if (Lex.CurrentToken -> Kind != TokenKind::RParent)
            forNode -> Inc = ParseExpr();
    }
    Lex.ExceptToken(TokenKind::RParent);
    forNode -> Stmt = ParseStatement();
    return forNode;
}

std::shared_ptr<AstNode> Parser::ParseBreakStmt() {
    auto breakNode = std::make_shared<BreakStmtNode>(Lex.CurrentToken);
    Lex.ExceptToken(TokenKind::Break);
    Lex.ExceptToken(TokenKind::Semicolon);
    return breakNode;
}

std::shared_ptr<AstNode> Parser::ParseReturnStmt() {
    auto returnNode = std::make_shared<ReturnStmtNode>(Lex.CurrentToken);
    Lex.ExceptToken(TokenKind::Return);
    returnNode -> Lhs = ParseExpr();
    auto varExprNode = GetVarExprNode(returnNode->Lhs);
    returnNode ->ReturnVarExpr = varExprNode;
    CurFuncReturnNode.push_back(returnNode);
    Lex.ExceptToken(TokenKind::Semicolon);
    return returnNode;
}

std::shared_ptr<AstNode> Parser::ParseContinueStmt() {
    auto continueNode = std::make_shared<ContinueStmtNode>(Lex.CurrentToken);
    Lex.ExceptToken(TokenKind::Continue);
    Lex.ExceptToken(TokenKind::Semicolon);
    return continueNode;
}

//"typedef" ("char" | "short" | "int" | "long" | "float" | "double" | "struct{" (field1,)+ "}")+ aliasName
std::shared_ptr<AstNode> Parser::ParseTypeDef() {
    auto emptyNode = std::make_shared<EmptyNode>(Lex.CurrentToken);
    if (Lex.CurrentToken->Kind == TokenKind::TypeDef){
        Lex.ExceptToken(TokenKind::TypeDef);
        auto tokens = std::list<std::shared_ptr<Token>>();
        auto type = ParseDeclarator(ParseDeclarationSpec(nullptr),&tokens);
        if(Scope::GetInstance() -> FindTag( Lex.CurrentToken->Content)){
            DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"typedef repeated define!");
        }
        for (auto token:tokens) {
            auto newType = std::make_shared<AliasType>(type,token);
            Scope::GetInstance() ->PushTag(token->Content,newType);
        }
        Lex.ExceptToken(TokenKind::Semicolon);
        return emptyNode;
    }
    return nullptr;
}

std::shared_ptr<AstNode> Parser::ParseWhileStmt() {
    auto whileNode = std::make_shared<WhileStmtNode>(Lex.CurrentToken);
    Lex.GetNextToken();
    Lex.ExceptToken(TokenKind::LParent);
    whileNode -> Cond = ParseExpr();
    Lex.ExceptToken(TokenKind::RParent);
    whileNode -> Then = ParseStatement();
    return whileNode;
}

std::shared_ptr<AstNode> Parser::ParseBlock() {
    Scope::GetInstance() -> PushScope("");
    auto blockNode = std::make_shared<BlockStmtNode>(Lex.CurrentToken);
    Lex.GetNextToken();
    while (Lex.CurrentToken->Kind != TokenKind::RBrace){
        blockNode -> Stmts.push_back(ParseStatement());
    }
    if(auto returnNode = std::dynamic_pointer_cast<ReturnStmtNode>(blockNode->Stmts.back())){
        blockNode -> Stmts.pop_back();
        blockNode->ReturnNode = returnNode;
    }
    Lex.ExceptToken(TokenKind::RBrace);
    Scope::GetInstance() -> PopScope();
    return blockNode;
}

//exampleCode : extern int add(int a,int b);
bool Parser::ParseExtern() {
    if (Lex.CurrentToken -> Kind == TokenKind::Extern){
        Lex.GetNextToken();
        std::shared_ptr<Attr> funcAttr = std::make_shared<Attr>();
        auto returnType =  ParseDeclarationSpec(funcAttr);
        auto funcName = Lex.CurrentToken->Content;
        auto funcType = std::dynamic_pointer_cast<FunctionType>(ParseDeclarator(returnType, nullptr));
        funcType -> FuncAttr = funcAttr;
        auto funcSign = std::make_shared<FuncSign>(funcType);
        funcSign ->FuncName = funcName;
        Scope::GetInstance() ->PushFuncSign(funcSign);
        Lex.ExceptToken(TokenKind::Semicolon);
        return true;
    }
    return false;
}

//ParseCompoundStmt ::= "{" ParseInitListExpr | Stmt |  StmtList  "}"
std::shared_ptr<AstNode> Parser::ParseCompoundStmt() {
    auto initCstNode = ParseInitListExpr();
    if(initCstNode)
        return initCstNode;
    if (Lex.CurrentToken ->Kind == TokenKind::LBrace){
        Scope::GetInstance() -> PushScope("");
        auto node = std::make_shared<StmtExprNode>(Lex.CurrentToken);
        Lex.GetNextToken();
        while (Lex.CurrentToken -> Kind != TokenKind::RBrace){
            node -> Stmts.push_back(ParseStatement());
        }
        Lex.GetNextToken();
        Lex.ExceptToken(TokenKind::RParent);
        Scope::GetInstance() -> PopScope();
        return node;
    }
    return nullptr;
}

