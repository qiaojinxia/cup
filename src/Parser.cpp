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
        auto multiAssignNode = std::make_shared<DeclarationAssignmentStmtNode>(Lex.CurrentToken);
        std::list<std::shared_ptr<AssignNode>> assignNodes;
        std::list<std::shared_ptr<ExprVarNode>> declarationNodes;
        std::shared_ptr<Attr> varAttr = std::make_shared<Attr>();
        std::shared_ptr<Type> baseType = ParseDeclarationSpec(varAttr);
        //parse declaration statement
        while (auto din = ParseDeclarator(baseType)) {
            auto varNode = std::make_shared<Var>(din->Type,din->ID->Content);
            if (Scope::GetInstance()->IsRootScope() || varAttr->isStatic ){
                auto stVarNm = Scope::GetInstance() -> PushStaticVar(din->ID->Content,din->Type);
                varNode->GlobalName= stVarNm;
                varNode->VarAttr = varAttr;
                varNode->VarAttr->isInit = true;
                if (Scope::GetInstance()->IsRootScope() && !varAttr->isStatic)
                    varNode ->VarAttr->isGlobal = true;
                Scope::GetInstance() ->PushVar(varNode);
            }else{
                varNode = NewLocalVar(varNode->Name,varNode->Type,varNode->VarAttr);
            }
            auto exprVarNode = std::make_shared<ExprVarNode>(din->Tk);
            exprVarNode->VarObj = varNode;
            declarationNodes.push_back(exprVarNode);
            if (din->Value){
                auto assignNode = std::make_shared<AssignNode>(din->Tk);
                assignNode -> Lhs = exprVarNode;
                assignNode ->BinOp = BinaryOperator::Assign;
                assignNode ->Rhs = din->Value;
                assignNodes.push_back(assignNode);
            }

        }

//        if func pointer is int (*a)(int,int) = max; not int (*a)(int,int) = &max; Is the same meaning
//         so  we  need to auto convert to &max use unaryNode warp
//        auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(valueNode);
//        if (type ->IsFuncPointerType() && !unaryNode && valueNode ->Type->IsFunctionType() ){
//            auto unaryNode = std::make_shared<UnaryNode>(nullptr);
//            unaryNode ->Uop = UnaryOperator::Addr;
//            unaryNode ->Lhs = valueNode;
//            valueNode = unaryNode;
//        }
        for (auto &n:assignNodes){
            auto leftDec = std::dynamic_pointer_cast<ExprVarNode>(n->Lhs);
            //static var init value
            if (leftDec->VarObj->VarAttr->isStatic || Scope::GetInstance()->IsRootScope()){
                auto cstNode = std::dynamic_pointer_cast<ConstantNode>(n->Rhs);
                TypeVisitor typeVisitor;
                n  ->Accept(&typeVisitor);
                //if declaration rhs not a constNode for example  : static int * m = &m;
                if (!cstNode){
                    auto unaryNode = std::dynamic_pointer_cast<UnaryNode>(n->Rhs);
                    if(auto refVar = std::dynamic_pointer_cast<ExprVarNode>(unaryNode->Lhs)){
                        if (refVar && refVar->VarObj->VarAttr->isStatic){
                            auto snd = Scope::GetInstance()->GetStaticUnInitVar(leftDec->VarObj->GlobalName);
                            auto nNode = std::make_shared<ConstantNode>(nullptr);
                            nNode ->refStatic = refVar->VarObj->GlobalName;
                            nNode -> Type = unaryNode->Type;
                            snd ->Next = nNode;
                            cstNode = snd;
                        }
                    }
                }
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
            NextToken
            if (TokenEqualTo(LBrace)){
                node =  ParseCompoundStmt();
                break;
            }
            node = ParseExpr();
            ExceptToken(RParent);
            break;
        }
        case TokenKind::LBrace:
        {
            node =  ParseCompoundStmt();
            break;
        }
        case TokenKind::Identifier:
        {
            StoreLex(n1)
            NextToken
            if (TokenEqualTo(LParent)){
                ResumeLex(n1)
                auto funcCallNode =  ParseFuncCallNode();
                //to record funcall node if return is structType to distribution  memory in stack
                if (funcCallNode ->Type->GetBaseType()->IsRecordType())
                    CurFuncCall.push_back(std::dynamic_pointer_cast<FuncCallNode>(funcCallNode));
                return  funcCallNode;
            }
            ResumeLex(n1)
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
            NextToken
            break;
        }
        case  TokenKind::CharNum:
        {
            auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            constNode -> Value = Lex.CurrentToken -> Value;
            constNode -> Type = Type::CharType;
            NextToken
            constNode -> isChar = true;
            node =  constNode;
            break;
        }
       case TokenKind::Num :
       {
           auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
           constNode -> Value = Lex.CurrentToken -> Value;
           constNode -> Type = Type::IntType;
           NextToken
           node =  constNode;
           break;
       }
        case TokenKind::Long :
        {
            auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            constNode -> Value = Lex.CurrentToken -> Value;
            constNode -> Type = Type::LongType;
            NextToken
            node =  constNode;
            break;
        }
        case TokenKind::String:
        {
            auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            constNode -> Value = Lex.CurrentToken -> Value;
            constNode -> Type = Type::PtrCharType;
            NextToken
            node =  constNode;
            break;
        }
        case TokenKind::DoubleNum:
        case TokenKind::FloatNum:
        {
            auto constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            constNode -> Value = Lex.CurrentToken -> Value;
            if (constNode->Tk->Kind == TokenKind::FloatNum){
                constNode -> Type = Type::FloatType;
            }else if(constNode->Tk->Kind == TokenKind::DoubleNum){
                constNode -> Type = Type::DoubleType;
            }
            NextToken
            node =  constNode;
            break;
        }
        case TokenKind::SizeOf:
        {   //Accept type or var sizeof(int) |sizeof a
            auto sizeOfNode = std::make_shared<SizeOfExprNode>(Lex.CurrentToken);
            NextToken
            if (Lex.CurrentToken ->Kind == TokenKind::LParent){
                NextToken
                if (IsTypeName()){
                    std::shared_ptr<Attr> varAttr = std::make_shared<Attr>();
                    auto baseType = ParseDeclarationSpec(varAttr);
                    auto din = ParseDeclarator(baseType);
                    auto emptyNode= std::make_shared<EmptyNode>(nullptr);
                    if (din)
                        emptyNode ->Type = din->Type;
                    else
                        emptyNode ->Type = baseType;
                    sizeOfNode -> Lhs = emptyNode;
                    ExceptToken(RParent);
                }else{
                    sizeOfNode -> Lhs = ParseExpr();
                    ExceptToken(RParent);
                }
            }else{
                sizeOfNode -> Lhs = ParseCastExpr();
            }

            node =  sizeOfNode;
            break;
        }
       default:
           if (ThrowWaring)
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
    Scope::GetInstance() -> PushScope(ROOTSCOPE);
    auto node = std::make_shared<ProgramNode>(Lex.CurrentToken);
    LocalVars = &node -> Global;
    while (Lex.CurrentToken -> Kind != TokenKind::Eof){
        if (ParseExtern()){
            continue;
        }
        if(ParseTypeDef()){
            continue;
        }
        if (auto funcNode = IsFunc()){
            node ->Funcs.push_back(ParseFunc(funcNode));
        }

        if(auto declNode = ParseDeclarationExpr()){
            node->DeclarationNode.push_back(declNode);
            ExceptToken(Semicolon);
        }
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
            ExceptToken(Semicolon);
            return exprNode;
        }
    }
}

std::shared_ptr<Var> Parser::FindLocalVar(std::string_view varName) {
    return Scope::GetInstance() -> FindVar(varName);
}

std::shared_ptr<Var> Parser::NewLocalVar(std::string_view varName,std::shared_ptr<Type> type,std::shared_ptr<Attr> attr) {
    if (!Scope::GetInstance()){
        return nullptr;
    }
    auto obj = std::make_shared<Var>(type);
    obj ->Name = varName;
    obj -> Offset = 0;
    obj -> VarAttr  = attr;
    LocalVars -> push_back(obj);
    Scope::GetInstance() -> PushVar(obj);
    return obj;
}

std::shared_ptr<ExprVarNode> Parser::GetVarExprNode(const std::shared_ptr<AstNode>& node) {
    auto exprVarNode = std::dynamic_pointer_cast<ExprVarNode>(node);
    return exprVarNode;
}


std::shared_ptr<AstNode> Parser::ParseFunc(std::shared_ptr<FunctionNode> node) {
    //if not func impl return null
    if (Lex.CurrentToken->Kind != TokenKind::LBrace){
        return nullptr;
    }
    LocalVars = &node -> Locals;
    Scope::GetInstance() -> PushScope(node -> FuncName);
    auto funcType = std::dynamic_pointer_cast<FunctionType>(node->Type);
    for(auto it = funcType -> Params.rbegin();it != funcType -> Params.rend();++it) {
            //if paramer is arry actually is pointer
        if ((*it)->Type->IsArrayType()) {
            auto  pType = std::make_shared<PointerType>((*it)->Type);
            node->Params.push_front(NewLocalVar((*it)->TToken->Content, pType,(*it)->ParamAttr));
        } else {
            node->Params.push_front(NewLocalVar((*it)->TToken->Content, (*it)->Type, (*it)->ParamAttr));
        }
    }
    TypeVisitor typeVisitor;

    ExceptToken(LBrace);

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
            if (node->Type->GetBaseType()->IsRecordType()){
                rtStmt ->ReturnOffset = -1 * Type::VoidType->Size;
            }
            node -> ReturnStmts.push_back(rtStmt);
            if (rtStmt -> ReturnVarExpr)
                node ->ReturnVarMap[rtStmt -> ReturnVarExpr ->Name] = rtStmt -> ReturnVarExpr;
            rtStmt -> Type = funcType->ReturnType;
            rtStmt ->Accept(&typeVisitor);
            if (!Type::IsTypeEqual(funcType->ReturnType, rtStmt->Type)){
                auto tips =  string_format("excepted return type %s  get type %s !", funcType->ReturnType->Align, rtStmt->Type->Align);
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

    auto funcSign = std::make_shared<FuncSign>(funcType);
    funcSign ->FuncName = node ->FuncName;

    if (Scope::GetInstance() ->GetFuncSign(funcSign ->FuncName)){
        auto tips =  string_format("redefinition of '%s' !",std::string(funcSign ->FuncName).c_str());
        DiagLoc(Lex.SourceCode, node->Tk->Location,tips.c_str());
    }

    if (node->ReturnStmts.empty() && node->Type->GetBaseType() != Type::VoidType)
        DiagLoc(Lex.SourceCode,node->Tk->Location, string_format("func %s excepted return ",std::string(funcSign ->FuncName).data()).data());

    Scope::GetInstance() ->PushFuncSign(funcSign);

    Scope::GetInstance() -> PopScope();
    ExceptToken(RBrace);
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
    ExceptToken(Identifier)
    ExceptToken(LParent)
    int i = 0;
    if (funcType->IsFunctionType() && funcType->GetBaseType()->IsRecordType())
        i = 1; //if return struct first args is return struct write to address
    while(Lex.CurrentToken->Kind != TokenKind::RParent){
        if (Lex.CurrentToken ->Kind == TokenKind::Comma){
            NextToken
        }
        auto argNType = funcType ->Params[i]->Type;
        TypeVisitor typeVisitor;
        auto arg = ParseExpr();
        arg ->Accept(&typeVisitor);
        if (!Type::IsTypeEqual(argNType, arg->Type)){
            arg = CastNodeType(arg->Type,argNType,arg);
            if (arg->Type != argNType){
                auto tips =  string_format("Input type of parameter is incorrect. It should be %s "
                                           "instead of %s. Please check!", argNType->Alias, arg->Type->Alias);
                DiagLoc(Lex.SourceCode, arg->Tk->Location,tips.c_str());
            }
        }
        node -> Args.push_back(arg);
    }

    node -> Type = funcType;
    //Check Func args
    ExceptToken(RParent)
    return node;
}

//ParseDeclarationSpec :=  ( int ｜ char | short | long | union | struct | union | float | double | const | * | static )  ParseDeclarationSpec |  ε
std::shared_ptr<Type> Parser::ParseDeclarationSpec(const std::shared_ptr<Attr>& attr) {
    bool isConstant = false;
    int baseType = 0;
    std::shared_ptr<Type> sType;
    std::shared_ptr<Type> type;
    while(true){
        if (TokenEqualTo(Const)){
            isConstant = true;
            NextToken
            continue;
        }else if (TokenEqualTo(Static)){
            if (!attr)
               ASSERT("invalid keyword !");
            attr->isStatic = true;
            NextToken
            continue;
        }else if (TokenEqualTo(Void)){
            NextToken
            baseType = (int) BuildInType::Kind::Void;
            continue;
        }else if (TokenEqualTo(Int)){
            NextToken
            baseType += (int) BuildInType::Kind::Int;
            continue;
        }else if(TokenEqualTo(Char)){
            NextToken
            baseType += (int) BuildInType::Kind::Char;
            continue;
        }else if(TokenEqualTo(Short)){
            NextToken
            baseType += (int) BuildInType::Kind::Short;
            continue;
        }else if(TokenEqualTo(Long)){
            NextToken
            baseType += (int) BuildInType::Kind::Long;
            continue;
        }else if(TokenEqualTo(Struct)){
            NextToken
            sType = ParseStructDeclaration();
           continue;
        }else if(TokenEqualTo(Union)){
            NextToken
            sType =  ParseUnionDeclaration();
            continue;
        }else if(TokenEqualTo(Float)){
            NextToken
            baseType += (int) BuildInType::Kind::Float;
            continue;
        }else if(TokenEqualTo(Double)){
            NextToken
            baseType += (int) BuildInType::Kind::Double;
            continue;
        }else if(TokenEqualTo(_Bool)){
            NextToken
            baseType += (int) BuildInType::Kind::Bool;
            continue;
        }else if(TokenEqualTo(Asterisk)){
            NextToken
            if (sType){
                if (isConstant){
                    sType = std::make_shared<AliasType>(sType,Lex.CurrentToken);
                    sType->constant = true;
                }
                sType = std::make_shared<PointerType>(sType);
            }else{
                sType = std::make_shared<PointerType>(GenerateType(baseType,isConstant));
                baseType = 0;
            }
            isConstant = false;
            continue;
        }else if(TokenEqualTo(SIGNED)){
            NextToken
            baseType |= (int) BuildInType::Kind::Signed;
            continue;
        }else if(TokenEqualTo(UNSIGNED)){
            NextToken
            baseType |= (int) BuildInType::Kind::UnSigned;
            continue;
        }else if(TokenEqualTo(Enum)){
            NextToken
            auto enumType = std::make_shared<EnumType>(Lex.CurrentToken);
            return enumType;
        }else{
            if (sType)
                break;
            if (baseType > BuildInType::Kind::Void)
                break;
            type = Scope::GetInstance() -> FindTag(Lex.CurrentToken ->Content);
            if (type){
                if (isConstant){
                    type = std::make_shared<AliasType>(type,Lex.CurrentToken);
                    type -> constant = true;
                }
                isConstant = false;
                sType = type;
                NextToken
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
    //if const a = 3; default type is int
    if (isConstant && baseType ==0 ){
        baseType = (int) BuildInType::Kind::Int;
    }
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
            type = Type::UCharType;
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
    if (TokenEqualTo(LParent)){
        auto funcType = std::make_shared<FunctionType>(baseType);
        //if return type is struct  set the first param is  struct offset
        if(funcType->GetBaseType()->IsRecordType()){
            auto param = std::make_shared<Param>();
            param -> Type = std::make_shared<RecordType>();
            param ->TToken = std::make_shared<Token>();
            param ->TToken ->Content = "r_st";
            std::shared_ptr<Attr> paramAttr = std::make_shared<Attr>();
            param -> ParamAttr = paramAttr;
            funcType -> Params.push_back(param);
        }
        NextToken
        std::list<std::shared_ptr<DeclarationInfoNode>> diNodes;
        std::shared_ptr<Attr> paramAttr = std::make_shared<Attr>();
        if (TokenNotEqualTo(RParent)){
            if(paramAttr ->isStatic)//param can't be static
                ASSERT("invalid storage class specifier in function declarator")
                while(auto din = ParseDeclarator(ParseDeclarationSpec(paramAttr))){
                    diNodes.push_back(din);
                }
        }
        for (auto  &din:diNodes) {
            auto param = std::make_shared<Param>();
            param ->Type = din->Type;
            param ->ParamAttr = paramAttr;
            param -> TToken = din->Tk;
            funcType -> Params.push_back(param);
        }
        ExceptToken(RParent)
        return funcType;
    }else if(TokenEqualTo(LBracket)){
        NextToken
        if (TokenEqualTo(RBracket)){
            NextToken
            auto type = ParseTypeSuffix(baseType);
            return std::make_shared<ArrayType>(type,0);
        }
        int num = Lex.CurrentToken -> Value;
        ExceptToken(Num)
        ExceptToken(RBracket)
        auto type = ParseTypeSuffix(baseType);
        return std::make_shared<ArrayType>(type,num);
    }
  return baseType;
}

//ParseIdentifier ::= id | id = xxx | (* [id]?)
std::shared_ptr<DeclarationInfoNode> Parser::ParseIdentifier(std::shared_ptr<Type> baseType){
    auto type = baseType;
    auto din = std::make_shared<DeclarationInfoNode>(Lex.CurrentToken);
    if (TokenEqualTo(LParent)){
        NextToken
        bool pointer = false;
        if (TokenEqualTo(Asterisk)){
            pointer = true;
            NextToken
        }
        StoreLex(n1)
        if (TokenNotEqualTo(RParent))
            ParseIdentifier(type);
        ExceptToken(RParent)
        type = ParseTypeSuffix(type);
        StoreLex(n2)
        ResumeLex(n1)
        din =  ParseIdentifier(type);
        ResumeLex(n2)
        if (pointer)
            din -> Type = std::make_shared<PointerType>(din -> Type);
    }else if(TokenEqualTo(Identifier)){
        if (TokenEqualTo(Asterisk)){
            type = std::make_shared<PointerType>(type);
            NextToken
        }
        din ->ID = Lex.CurrentToken;
        ExceptToken(Identifier)
        din ->Type = ParseTypeSuffix(type);
    }else if(TokenEqualTo(RParent) || TokenEqualTo(LBracket)){
        din ->Type = ParseTypeSuffix(type);
        return din;
    }else{
       return nullptr;
    }
    if (Lex.CurrentToken->Kind == TokenKind::Assign){
        NextToken
        din ->Value = ParseExpr();
    }
    return din;
}

//  ParseDeclarator ::=  (Identifier)* ParseTypeSuffix
std::shared_ptr<DeclarationInfoNode> Parser::ParseDeclarator(std::shared_ptr<Type> baseType) {
    if (TokenEqualTo(Comma)){
        return nullptr;
    }
    std::shared_ptr<DeclarationInfoNode> din;
    if (TokenEqualTo(Identifier) || TokenEqualTo(Asterisk)
    || TokenEqualTo(LParent) || TokenEqualTo(LBracket) ){
        if (TokenEqualTo(Asterisk)){
            NextToken
            auto ptrType = std::make_shared<PointerType>(baseType);
            din = ParseIdentifier(ptrType);
        }else{
            din = ParseIdentifier(baseType);
        }
        if (TokenEqualTo(Comma))
            NextToken
        return din;
    }
    return nullptr;
}

//ParseUnaryExpr ::= (+ | - | * | & | ～ ｜ ! )? ParseCastExpr | ParsePostFixExpr
std::shared_ptr<AstNode> Parser::ParseUnaryExpr() {
    auto node = std::make_shared<UnaryNode>(Lex.CurrentToken);
    int UnaryOp = 0 ;
    while (true){
        if (TokenEqualTo(Plus)){
        }else if(TokenEqualTo(Minus)){
            UnaryOp ^= (int)UnaryOperator::Minus;
        }else{
            break;
        }
        NextToken
    }
    if (!UnaryOp){
        switch (Lex.CurrentToken -> Kind){
            case TokenKind::Plus:
                break;
            case TokenKind::Minus:
                break;
            case TokenKind::Asterisk:
                UnaryOp += (int)UnaryOperator::Deref;
                break;
            case TokenKind::Amp:
                UnaryOp += (int)UnaryOperator::Addr;
                break;
            case TokenKind::Tilde:
                UnaryOp += (int)UnaryOperator::BitNot;
                break;
            case TokenKind::PPlus:
                UnaryOp += (int)UnaryOperator::Incr;
                break;
            case TokenKind::MMinus:
                UnaryOp += (int)UnaryOperator::Decr;
                break;
            case TokenKind::ExclamationMark:
                UnaryOp += (int)UnaryOperator::Not;
                break;
            default:
                goto End;
        }
        NextToken
    }
    End:
    if (!UnaryOp){
        return ParsePostFixExpr();
    }
    node ->Uop = (UnaryOperator)UnaryOp;
    node -> Lhs = ParseCastExpr();
    //if -n n is constantNode mul -1 to constantNode
    if (node->Uop == UnaryOperator::Minus){
        if (auto cstNode = std::dynamic_pointer_cast<ConstantNode>(node -> Lhs)){
            if (cstNode->Type->IsFloatType()){
                cstNode->Value |= 0x80000000;
                cstNode ->isModify = true;
            }else if(cstNode->Type->IsDoubleType()){
                cstNode->Value |= 0x8000000000000000;
                cstNode ->isModify = true;
            }else{
                cstNode->Value  *= -1;
                cstNode ->isModify = true;
                cstNode->CastValue(std::dynamic_pointer_cast<BuildInType>(cstNode->Type));
            }
            return node->Lhs;
        }
    }
    TypeVisitor typeVisitor;
    node->Accept(&typeVisitor);
    return node;
}

std::shared_ptr<AstNode> Parser::ParsePostFixArray(std::shared_ptr<AstNode> left) {
    if(TokenEqualTo(LBracket)){
        auto addNode = std::make_shared<AddNode>(Lex.CurrentToken);
        NextToken
        addNode ->BinOp = BinaryOperator::Add;
        addNode ->Lhs = left;
        addNode ->Rhs = ParsePrimaryExpr();
        if (TokenEqualTo(RBracket))
            NextToken
        auto deferNode = std::make_shared<UnaryNode>(Lex.CurrentToken);
        deferNode ->Uop = UnaryOperator::Deref;
        deferNode ->Lhs = addNode;
        return ParsePostFixArray(deferNode);
    }
    return left;
}

//ParsePostFixExpr ::= ParsePrimaryExpr ("++" | "--" ｜ "->" ident | "." ident ｜ "[" ParseExpr "]" )*
std::shared_ptr<AstNode> Parser::ParsePostFixExpr() {
    auto left = ParsePrimaryExpr();
    if (auto constNode = std::dynamic_pointer_cast<ConstantNode>(left)){
        Scope::GetInstance() -> PutToConstantTable(constNode);
    }
    while (true){
        if (TokenEqualTo(LParent)){
            return ParseFuncCallNode();
        }else if (TokenEqualTo(LBracket)){
            auto arrayMemberNode = std::make_shared<ArrayMemberNode>(Lex.CurrentToken);
            auto deferArrayNode = std::make_shared<UnaryNode>(Lex.CurrentToken);
            arrayMemberNode ->Lhs = ParsePostFixArray(left);
            left = arrayMemberNode;
            continue;
        }else if(TokenEqualTo(Period)){
            auto memberNode = std::make_shared<MemberAccessNode>(Lex.CurrentToken);
            NextToken
            memberNode -> Lhs = left;
            memberNode -> fieldName = Lex.CurrentToken -> Content;
            left = memberNode;
            ExceptToken(Identifier)
            continue;
        }else if(TokenEqualTo(PointerTo)){
            auto deferNode = std::make_shared<UnaryNode>(Lex.CurrentToken);
            NextToken
            deferNode -> Uop = UnaryOperator::Deref;
            deferNode -> Lhs = left;
            auto memberNode = std::make_shared<MemberAccessNode>(Lex.CurrentToken);
            memberNode -> Lhs = deferNode;
            memberNode -> fieldName = Lex.CurrentToken->Content;
            ExceptToken(Identifier)
            left = memberNode;
            break;
        }else if(TokenEqualTo(PPlus) || TokenEqualTo(MMinus)){
            auto token = Lex.CurrentToken;
            NextToken
            if (left ->Type->IsConstant()){
                DiagLoc(Lex.SourceCode, Lex.GetLocation(), "constant can't change!");
            }
            int incrSize = 1;
            auto cstNode  =  std::make_shared<ConstantNode>(nullptr);
            auto tp = left ->Type;
            if (left ->Type->IsPointerType()){
                incrSize = left ->Type->GetBaseType()->Size;
                tp =  Type::LongType;
            }else if(left ->Type->IsFloatPointNum()){
                tp = left ->Type;
            }else{
                tp = Type::IntType;
            }
            cstNode -> Value = incrSize;
            cstNode ->CastValue(std::dynamic_pointer_cast<BuildInType>(tp));
            if (token->Kind  == TokenKind::PPlus){
                auto postIncrNode = std::make_shared<IncrNode>(token);
                postIncrNode -> Lhs = left;
                postIncrNode -> Rhs = cstNode;
                postIncrNode -> BinOp = BinaryOperator::Incr;
                left = postIncrNode;
            }else{
                auto postDcrNode = std::make_shared<DecrNode>(token);
                postDcrNode -> Lhs = left;
                postDcrNode -> Rhs = cstNode;
                postDcrNode -> BinOp = BinaryOperator::Decr;
                left = postDcrNode;
            }
            if ( cstNode ->Type->IsFloatPointNum())
                Scope::GetInstance() -> PutToConstantTable(cstNode);
            break;
        }else{
            break;
        }
    }
    return left;
}

std::shared_ptr<Type> Parser::ParseUnionDeclaration() {
    std::shared_ptr<std::string_view> stName;
    auto unionDeclaration = ParseRecord(RecordType::TagKind::Union, stName);
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
    std::shared_ptr<std::string_view> stName;
    auto structDeclaration = ParseRecord(RecordType::TagKind::Struct,stName);
    if (structDeclaration->fields.empty())
        return structDeclaration;
    int offset = 0;
    //set struct field  align
    for (auto &field : structDeclaration ->fields) {
        offset = AlignTo(offset,field -> type ->Align);
        field -> Offset = offset;
        offset += field -> type ->Size;
        if (structDeclaration -> Align < field ->type ->Align){
            structDeclaration -> Align = field ->type -> Align;
        }
    }
    //if empty struct is typedef  but not init fields , after init need to reset symbol table which  pointer to record
    structDeclaration -> Size = AlignTo(offset, structDeclaration ->Align);
    if (stName){
       auto oldAliasType=  std::dynamic_pointer_cast<AliasType>(Scope::GetInstance()->FindTag(*stName));
            Scope::GetInstance() -> PushTag(*stName,
                                            std::make_shared<AliasType>(oldAliasType->GetBaseType(),oldAliasType->token));
    }
    return structDeclaration;
}

std::shared_ptr<RecordType> Parser::ParseRecord(RecordType::TagKind recordeType,std::shared_ptr<std::string_view>& name) {
    std::shared_ptr<std::string_view> recordName;
    auto rtype = std::make_shared<RecordType>();
    rtype ->Kind = recordeType;
    auto _rtype = std::make_shared<RecordType>();
    std::shared_ptr<Type> sRecord;
    if (TokenEqualTo(Identifier)){
        recordName = std::make_shared<std::string_view>(Lex.CurrentToken->Content);
        NextToken
        sRecord = Scope::GetInstance() -> FindTag(*recordName);
        //to handle typedef struct T T; struct T { int x; }; sizeof(T); typedef T add to symbol and define T cover it
        if (sRecord && sRecord ->IsAliasType()){
            _rtype = std::dynamic_pointer_cast<RecordType>(sRecord->GetBaseType());
            name = recordName;
        }else{
            _rtype = std::dynamic_pointer_cast<RecordType>(sRecord);
        }
        //when pre define a pointer struct T *next; but record not define specific fields
        // it maybe defines after we can later to init record fields
        // such as : struct T *next; struct T { struct T *next; int x; } a;
        if (_rtype && rtype->fields.empty())
            rtype = _rtype;
        if (!sRecord)
            Scope::GetInstance() -> PushTag(*recordName,rtype);
    }
    if (Lex.CurrentToken -> Kind ==  TokenKind::LBrace){
        NextToken
        while(Lex.CurrentToken  -> Kind != TokenKind::RBrace){
            auto type = ParseDeclarationSpec(nullptr);
            while(auto din = ParseDeclarator(type)){
                rtype ->fields.push_back(std::make_shared<Field>(din->Type,din->ID,0));
            }
            ExceptToken(Semicolon)
        }
        ExceptToken(RBrace)
    }else {
        if (!recordName){
            DiagLoc(Lex.SourceCode,Lex.CurrentToken->Location,"except declaration struct/union name!");
        }
        if (rtype){
            return rtype;
        }
    }
    return rtype;
}

std::shared_ptr<AstNode> Parser::ParseBinaryExpr(int priority) {
    TypeVisitor typeVisitor;
    auto leftNode  = ParseCastExpr();
    while(true){
        auto oPrior = TopPrecedenceTable.find(Lex.CurrentToken->Kind);
        if (oPrior->first == TopPrecedenceTable.end()->first)
            break;
        switch(oPrior->second.associativity){
            case Associativity::LeftAssociative:
                if (oPrior->second.priority >= priority){
                    break;
                }
            case Associativity::RightAssociative:
                if (oPrior->second.priority > priority){
                    break;
                }
        }
        switch (Lex.CurrentToken->Kind) {
            case TokenKind::Plus:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Add);
                break;
            case TokenKind::PlusAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Add);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
            case TokenKind::Minus:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Minus);
                break;
            case TokenKind::MinusAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Minus);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
            case TokenKind::Asterisk:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Mul);
                break;
            case TokenKind::AsteriskAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Mul);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
            case TokenKind::Slash:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::IDiv);
                break;
            case TokenKind::SlashAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::IDiv);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
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
            case TokenKind::ModAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::Mod);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
            case TokenKind::VerticalBar:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitOr);
                break;
            case TokenKind::VerticalBarAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitOr);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
            case TokenKind::Caret:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitXor);
                break;
            case TokenKind::CaretAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitXor);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
            case TokenKind::Sal:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitSal);
                break;
            case TokenKind::SalAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitSal);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
            case TokenKind::Sar:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitSar);
                break;
            case TokenKind::SarAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitSar);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
            case TokenKind::Amp:
                leftNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitAnd);
                break;
            case TokenKind::AmpAssign:
            {
                auto rightNode = ParseBinaryOperationExpr(leftNode,BinaryOperator::BitAnd);
                leftNode = Assign(leftNode,rightNode);
                break;
            }
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


std::shared_ptr<AssignNode> Parser::Assign(std::shared_ptr<AstNode> left,const std::shared_ptr<AstNode>& right){
    auto assignNode = std::make_shared<AssignNode>(Lex.CurrentToken);
    assignNode -> Lhs = left;
    assignNode -> Rhs = right;
    if (std::dynamic_pointer_cast<ConstantNode>(right)){
        assignNode ->isConstantAssign = true;
    }
    //if assign rhs is func convert to &func
    if (auto exprNode = std::dynamic_pointer_cast<ExprVarNode>(assignNode ->Rhs)){
        if(Scope::GetInstance()->GetFuncSign(exprNode->Name)){
            auto  unaryNode = std::make_shared<UnaryNode>(exprNode->Tk);
            unaryNode ->Lhs = assignNode->Rhs;
            unaryNode->Uop = UnaryOperator::Addr;
            assignNode ->Rhs = unaryNode;
        }
    }
    return assignNode;
}

std::shared_ptr<AstNode> Parser::ParseBinaryOperationExpr(const std::shared_ptr<AstNode>& left, BinaryOperator op) {
    auto curPriority =  TopPrecedenceTable.find(Lex.CurrentToken->Kind)->second.priority;
    NextToken
    std::shared_ptr<BinaryNode> binaryNode;
    switch (op){
        case BinaryOperator::Assign:
        {

            binaryNode = Assign(left,ParseBinaryExpr(curPriority));
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

bool Parser::IsTypeName() const {
    if (TokenEqualTo(Int) || TokenEqualTo(Char)
        || TokenEqualTo(Short)  || TokenEqualTo(Long)
        || TokenEqualTo(Float)  || TokenEqualTo(Double)
        || TokenEqualTo(Struct) || TokenEqualTo(Union)
        || TokenEqualTo(SIGNED) || TokenEqualTo(UNSIGNED)
        || TokenEqualTo(_Bool)  || TokenEqualTo(Enum)
        || TokenEqualTo(Const)  || TokenEqualTo(Static)
        || TokenEqualTo(Void)){
        return true;
    }
    if(auto symbolType = Scope::GetInstance() -> FindTag(Lex.CurrentToken->Content)){
        //if record type not use typedef although type add to symbol table but still can't as typename
        if (!symbolType->isTypedef)
            return false;
        return true;
    }
    return false;
}

//ParseCastExpr ::= "(" type-name ")" ParseCastExpr | ParseUnaryExpr
std::shared_ptr<AstNode> Parser::ParseCastExpr() {
    StoreLex(n1)
    if (TokenEqualTo(LParent)){
        auto token = Lex.CurrentToken;
        NextToken
        if (IsTypeName()){
            auto type = ParseDeclarationSpec(nullptr);
            ExceptToken(RParent)
            auto lNode = ParseCastExpr();
            auto castNode = CastNodeType(lNode->Type,type,lNode);
            if (std::dynamic_pointer_cast<CastNode>(castNode))
                castNode-> Tk = token;
            return castNode;
        }else{
            ResumeLex(n1)
        }
    }
    return ParseUnaryExpr();
}

//{xx,xx,xx}
std::shared_ptr<ConstantNode> Parser::parseInitListExpr(std::shared_ptr<ConstantNode> root) {
    if(!root){
        root = std::make_shared<ConstantNode>(nullptr);
        root ->isRoot = true;
    }
    std::shared_ptr<ConstantNode> cursor= root;
    do {
        if (Lex.CurrentToken->Kind == TokenKind::Comma){
            NextToken
        }
        if (Lex.CurrentToken ->Kind == TokenKind::LBrace){
            NextToken
            std::shared_ptr<ConstantNode> nextNode = std::make_shared<ConstantNode>(nullptr);
            nextNode ->isRoot = true;
            parseInitListExpr(nextNode);
            cursor -> Next = nextNode;
            nextNode ->Sub = nextNode->Next;
            ExceptToken(RBrace)
        }else {
            if (!IsConstant()){
                return nullptr;
            }
            cursor->Next  = std::dynamic_pointer_cast<ConstantNode>(ParsePrimaryExpr());
            if (!cursor ->Next)
                return nullptr;
        }
        cursor = cursor -> Next;
    }while(Lex.CurrentToken->Kind == TokenKind::Comma);
    return root;
}

bool Parser::IsConstant() const {
    if (Lex.CurrentToken->Kind == TokenKind::CharNum ||Lex.CurrentToken->Kind == TokenKind::Num
    ||Lex.CurrentToken->Kind == TokenKind::String || Lex.CurrentToken->Kind == TokenKind::FloatNum){
        return true;
    }
    return false;
}
// ParseInitListExpr ::= "{" (ParsePrimaryExpr,",")* | (ParsePrimaryExpr:ParsePrimaryExpr ",")* "}"
std::shared_ptr<ConstantNode> Parser::ParseInitListExpr() {
    StoreLex(n1)
    if (TokenEqualTo(LBrace)){
        ExceptToken(LBrace)
        auto initCstNode = parseInitListExpr(nullptr);
        if (initCstNode){
            ExceptToken(RBrace)
            if(initCstNode ->HasSetValue()){
                return initCstNode;
            }
        }
        ResumeLex(n1)
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
    NextToken
    int count = 0;
    while(Lex.CurrentToken ->Kind == TokenKind::Identifier){
        auto enumName = Lex.CurrentToken;
        std::shared_ptr<ConstantNode> constNode;
        NextToken
        if (Lex.CurrentToken ->Kind == TokenKind::Assign){
            NextToken
            constNode = std::make_shared<ConstantNode>(Lex.CurrentToken);
            count = 0;
            NextToken
        }else{
            constNode = std::make_shared<ConstantNode>(nullptr);
            constNode ->Value = count;
            count += 1;
        }
        constNode ->Name = std::string(enumName->Content);
        constNode ->Type = enumType;
        Scope::GetInstance() ->PutToConstantTable(constNode);
        if (Lex.CurrentToken ->Kind == TokenKind::Comma){
            NextToken
        }else{
            break;
        }
    }
    Scope::GetInstance() ->PushTag(enumType->token->Content,enumType);
    ExceptToken(RBrace)
    return std::make_shared<EmptyNode>(Lex.CurrentToken);
}

//exampleCode  `a < 7 ? a = 1 : a = 0;`
//ParseTernaryExpr ::= condition "?" ParseExpr ":" ParseExpr;
std::shared_ptr<AstNode> Parser::ParseTernaryExpr(std::shared_ptr<AstNode> condition) {
    auto ternaryNode = std::make_shared<TernaryNode>(Lex.CurrentToken);
    ternaryNode -> Cond = std::move(condition);
    ExceptToken(QuestionMark)
    ternaryNode->Then = ParseExpr();
    ExceptToken(Colon)
    ternaryNode->Else = ParseExpr();
    return ternaryNode;
}

//example switch (7) { case 7: xxxx; break; case 6: xxxx;break;}
//ParseSwitchCaseStmt ::= "switch" "(" num ")" "{" (("case" | "default") ":" )+ statements  （"break")?  +  "}"
std::shared_ptr<AstNode> Parser::ParseSwitchCaseStmt() {
    bool isEmpty = true;
    auto switchCaseNode = std::make_shared<SwitchCaseSmtNode>(Lex.CurrentToken);
    ExceptToken(Switch)
    ExceptToken(LParent)
    switchCaseNode ->Value = ParseCastExpr();
    ExceptToken(RParent)
    ExceptToken(LBrace)
    auto branchMap = std::map<std::shared_ptr<AstNode>,std::list<std::shared_ptr<AstNode>>>();
    while (Lex.CurrentToken->Kind == TokenKind::Case || Lex.CurrentToken->Kind == TokenKind::Default ){
        auto statementsNode = std::list<std::shared_ptr<AstNode>>();
        std::shared_ptr<AstNode> branchCond;
        if (Lex.CurrentToken->Kind ==TokenKind::Case){
            NextToken
            branchCond = ParseCastExpr();
        }else if(Lex.CurrentToken->Kind ==TokenKind::Default){
            NextToken
        }
        ExceptToken(Colon)
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
    ExceptToken(RBrace)
    return switchCaseNode;
}

//exampleCode: if( a < num ){}else{}
//ParseIfElseStmt ::= "if" "(" ( ParseExpr() ("||","&&")* )+ ")"
std::shared_ptr<AstNode> Parser::ParseIfElseStmt() {
    auto node = std::make_shared<IfElseStmtNode>(Lex.CurrentToken);
    ExceptToken(If)
    ExceptToken(LParent)
    node ->Cond = ParseExpr();
    ExceptToken(RParent)
    if (Lex.CurrentToken ->Kind != TokenKind::RBrace){
        node -> Then = ParseBlock();
    }
    if (TokenEqualTo(Else)){
        NextToken
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
    NextToken
    doWhileNode -> Stmt = ParseStatement();
    ExceptToken(While)
    ExceptToken(LParent)
    doWhileNode -> Cond = ParseExpr();
    ExceptToken(RParent)
    return doWhileNode;
}

std::shared_ptr<AstNode> Parser::ParseForStmt() {
    auto forNode = std::make_shared<ForStmtNode>(Lex.CurrentToken);
    ExceptToken(For)
    ExceptToken(LParent)
    if (Lex.CurrentToken->Kind != TokenKind::Semicolon){
        forNode -> Init = ParseExpr();
        ExceptToken(Semicolon)
        if (Lex.CurrentToken -> Kind !=TokenKind::Semicolon)
            forNode -> Cond = ParseExpr();
        ExceptToken(Semicolon)
        if (Lex.CurrentToken -> Kind != TokenKind::RParent)
            forNode -> Inc = ParseExpr();
    }
    ExceptToken(RParent)
    forNode -> Stmt = ParseStatement();
    return forNode;
}

std::shared_ptr<AstNode> Parser::ParseBreakStmt() {
    auto breakNode = std::make_shared<BreakStmtNode>(Lex.CurrentToken);
    ExceptToken(Break)
    ExceptToken(Semicolon)
    return breakNode;
}

std::shared_ptr<AstNode> Parser::ParseReturnStmt() {
    auto returnNode = std::make_shared<ReturnStmtNode>(Lex.CurrentToken);
    ExceptToken(Return)
    returnNode -> Lhs = ParseExpr();
    auto varExprNode = GetVarExprNode(returnNode->Lhs);
    returnNode ->ReturnVarExpr = varExprNode;
    CurFuncReturnNode.push_back(returnNode);
    ExceptToken(Semicolon)
    return returnNode;
}

std::shared_ptr<AstNode> Parser::ParseContinueStmt() {
    auto continueNode = std::make_shared<ContinueStmtNode>(Lex.CurrentToken);
    ExceptToken(Continue)
    ExceptToken(Semicolon)
    return continueNode;
}

//"typedef" ("char" | "short" | "int" | "long" | "float" | "double" | "struct{" (field1,)+ "}")+ aliasName
std::shared_ptr<AstNode> Parser::ParseTypeDef() {
    auto emptyNode = std::make_shared<EmptyNode>(Lex.CurrentToken);
    if (Lex.CurrentToken->Kind == TokenKind::TypeDef){
        ExceptToken(TypeDef)
        auto diNodes = ParseDeclarator(ParseDeclarationSpec(nullptr));
        while (auto din = ParseDeclarator(ParseDeclarationSpec(nullptr))){
            auto newType = std::make_shared<AliasType>(din->Type,din->ID);
            newType ->isTypedef = true;
            Scope::GetInstance() ->PushTag(din->ID->Content,newType);
        }
        if(Scope::GetInstance() -> FindTag( Lex.CurrentToken->Content)){
            ASSERT("typedef repeated define!")
        }
        ExceptToken(Semicolon)
        return emptyNode;
    }
    return nullptr;
}

std::shared_ptr<AstNode> Parser::ParseWhileStmt() {
    auto whileNode = std::make_shared<WhileStmtNode>(Lex.CurrentToken);
    NextToken
    ExceptToken(LParent)
    whileNode -> Cond = ParseExpr();
    ExceptToken(RParent)
    whileNode -> Then = ParseStatement();
    return whileNode;
}

std::shared_ptr<AstNode> Parser::ParseBlock() {
    Scope::GetInstance() -> PushScope("");
    auto blockNode = std::make_shared<BlockStmtNode>(Lex.CurrentToken);
    NextToken
    while (Lex.CurrentToken->Kind != TokenKind::RBrace){
        blockNode -> Stmts.push_back(ParseStatement());
    }
    if(auto returnNode = std::dynamic_pointer_cast<ReturnStmtNode>(blockNode->Stmts.back())){
        blockNode -> Stmts.pop_back();
        blockNode->ReturnNode = returnNode;
    }
    ExceptToken(RBrace)
    Scope::GetInstance() -> PopScope();
    return blockNode;
}

//exampleCode : extern int add(int a,int b);  typedef int (* PrintLove)();
bool Parser::ParseExtern() {
    if (TokenEqualTo(Extern)){
        NextToken
        std::shared_ptr<Attr> funcAttr = std::make_shared<Attr>();
        auto baseType =  ParseDeclarationSpec(funcAttr);
        while (auto din = ParseDeclarator(baseType)) {
            if (auto funcType = std::dynamic_pointer_cast<FunctionType>(din->Type)){
                funcType -> FuncAttr = funcAttr;
                auto funcSign = std::make_shared<FuncSign>(funcType);
                funcSign ->FuncName = din->ID->Content;
                Scope::GetInstance() ->PushFuncSign(funcSign);
            }
        }
        ExceptToken(Semicolon)
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
        NextToken
        while (Lex.CurrentToken -> Kind != TokenKind::RBrace){
            node -> Stmts.push_back(ParseStatement());
        }
        NextToken
        ExceptToken(RParent)
        Scope::GetInstance() -> PopScope();
        return node;
    }
    return nullptr;
}

std::shared_ptr<FunctionNode> Parser::IsFunc() {
    auto funcNode = std::make_shared<FunctionNode>(Lex.CurrentToken);
    StoreLex(n1)
    std::shared_ptr<Attr> varAttr = std::make_shared<Attr>();
    auto baseType = ParseDeclarationSpec(varAttr);
    if(TokenEqualTo(Identifier)){
        funcNode->FuncName = Lex.CurrentToken->Content;
        NextToken
    }else{
        ResumeLex(n1)
        return nullptr;
    }
    auto type = ParseTypeSuffix(baseType);
    if (!std::dynamic_pointer_cast<FunctionType>(type)){
        ResumeLex(n1)
        return nullptr;
    }
    funcNode -> Type = type;
    return funcNode;
}

