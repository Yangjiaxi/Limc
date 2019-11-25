%skeleton "lalr1.cc"
%require "3.4.2"
%defines
%define api.namespace { Limc }
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include "../S.0-Utils/token.h"

    namespace Limc {
        class Scanner;
        class Driver;
    }
}

%code top {
    #include "../S.0-Utils/driver.h"
    #include "../S.0-Utils/color.h"
    using namespace Limc;

    static Parser::symbol_type yylex(Scanner &scanner, Driver &driver) {
        return scanner.get_next_token();
    }
}

%lex-param { Scanner &scanner }
%lex-param { Driver &driver }
%parse-param { Scanner &scanner }
%parse-param { Driver &driver }
%locations
%define parse.trace
%define parse.error verbose

%type <Token> Program GlobalDeclDefList GlobalDeclDef Type VarList AssignmentExpr BlockStmt ParamList Param Stmt StmtList SwitchBodyStmt CaseStmtList CaseStmt DefaultStmt LocalDeclDef Expr CallExpr ArgumentList IndexExpr Index Assignable Identifier Literal ArrayLiteral ArrayItemList ArrayItem

%token END 0 "EOF"
%token PUNCTUATOR_PARENTHESIS_LEFT PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_BRACE_LEFT PUNCTUATOR_BRACE_RIGHT PUNCTUATOR_BRACKET_LEFT PUNCTUATOR_BRACKET_RIGHT PUNCTUATOR_SEMICOLON PUNCTUATOR_COMMA OPERATOR_QUESTION PUNCTUATOR_COLON
%token KEYWORD_FOR KEYWORD_IF KEYWORD_ELSE KEYWORD_WHILE KEYWORD_DO KEYWORD_RETURN KEYWORD_BREAK KEYWORD_CONTINUE KEYWORD_SWITCH KEYWORD_DEFAULT KEYWORD_CASE
%token <string> IDENTIFIER KEYWORD_TYPE LITERAL_FLOAT LITERAL_INTEGER LITERAL_CHAR LITERAL_STRING
%token <string> OPERATOR_SHIFT_RIGHT OPERATOR_SHIFT_LEFT OPERATOR_BITWISE_AND OPERATOR_BITWISE_OR OPERATOR_BITWISE_XOR OPERATOR_BITWISE_NOT
%token <string> OPERATOR_LOGICAL_NOT OPERATOR_LOGICAL_AND OPERATOR_LOGICAL_OR
%token <string> OPERATOR_COMPOUND_ASSIGNMENT OPERATOR_RELATIONAL OPERATOR_ASSIGNMENT
%token <string> OPERATOR_PLUS OPERATOR_MINUS OPERATOR_MULTIPLY OPERATOR_DIVIDE OPERATOR_MODULUS
%token <string> OPERATOR_INCREMENT OPERATOR_DECREMENT

%left OPERATOR_ASSIGNMENT OPERATOR_COMPOUND_ASSIGNMENT
%left OPERATOR_QUESTION PUNCTUATOR_COLON
%left OPERATOR_LOGICAL_OR
%left OPERATOR_LOGICAL_AND
%left OPERATOR_BITWISE_OR
%left OPERATOR_BITWISE_XOR
%left OPERATOR_BITWISE_AND
%left OPERATOR_RELATIONAL
%left OPERATOR_SHIFT_RIGHT OPERATOR_SHIFT_LEFT
%left OPERATOR_PLUS OPERATOR_MINUS
%left OPERATOR_MULTIPLY OPERATOR_DIVIDE OPERATOR_MODULUS

%right OPERATOR_LOGICAL_NOT OPERATOR_BITWISE_NOT

%nonassoc IF_END
%nonassoc KEYWORD_ELSE
%nonassoc OPERATOR_UNARY_MINUS OPERATOR_UNARY_PLUS
%nonassoc OPERATOR_INCREMENT OPERATOR_DECREMENT

%start Program

%%
Program:
    GlobalDeclDefList {
        driver.clear();
        $$ = Token("Program");
        $$.build_AST($1);
        driver.addToken($$);
        cout << driver.pretty_print() << endl;
    };
GlobalDeclDefList:
    GlobalDeclDefList GlobalDeclDef {
        $$ = $1;
        $$.build_AST($2);
    } | GlobalDeclDef {
        $$ = Token("GlobalDeclDefList");
        $$.build_AST($1);
    };
GlobalDeclDef:
    Type VarList PUNCTUATOR_SEMICOLON {
        $$ = Token("GlobalVarDecl");
        $$.build_AST($1);
        $$.build_AST($2);
    } | Type Assignable PUNCTUATOR_PARENTHESIS_LEFT ParamList PUNCTUATOR_PARENTHESIS_RIGHT BlockStmt {
        $$ = Token("FunctionDef");
        $$.build_AST($1);
        $$.build_AST($2);
        $$.build_AST($4);
        $$.build_AST($6);
    } | Type Assignable PUNCTUATOR_PARENTHESIS_LEFT ParamList PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_SEMICOLON {
        $$ = Token("FunctionDecl");
        $$.build_AST($1);
        $$.build_AST($2);
        $$.build_AST($4);
    } | error PUNCTUATOR_SEMICOLON {
        $$ = Token("ErrorStmt");
        yyerrok;
    };
Type:
    KEYWORD_TYPE {
        const string& value = $1;
        $$ = Token("Type", value);
    };
Assignable:
    Identifier {
        $$ = $1;
    } | IndexExpr {
        $$ = $1;
    };
Identifier:
    IDENTIFIER {
        $$ = Token("Identifier", $1);
    };
VarList:
    Assignable {
        $$ = Token("VarList");
        $$.build_AST($1);
    } | AssignmentExpr {
        $$ = Token("VarList");
        $$.build_AST($1);
    } | VarList PUNCTUATOR_COMMA Assignable {
        $$ = $1;
        $$.build_AST($3);
    } | VarList PUNCTUATOR_COMMA AssignmentExpr {
        $$ = $1;
        $$.build_AST($3);
    };
AssignmentExpr:
    Assignable OPERATOR_ASSIGNMENT Expr {
        $$ = Token("AssignmentExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Assignable OPERATOR_ASSIGNMENT ArrayLiteral {
        $$ = Token("AssignmentExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    };
ParamList:
    ParamList PUNCTUATOR_COMMA Param {
        $$ = $1;
        $$.build_AST($3);
    } | Param {
        $$ = Token("ParamList");
        $$.build_AST($1);
    } | {
        $$ = Token("ParamList");
    };
Param:
    Type Assignable {
        $$ = Token("ParamDecl");
        $$.build_AST($1);
        $$.build_AST($2);
    };
BlockStmt:
    PUNCTUATOR_BRACE_LEFT StmtList PUNCTUATOR_BRACE_RIGHT {
        $$ = $2;
    } | PUNCTUATOR_BRACE_LEFT PUNCTUATOR_BRACE_RIGHT {
        $$ = Token("BlockStmt");
    } | error PUNCTUATOR_BRACE_RIGHT {
        $$ = Token("ErrorStmt");
        yyerrok;
    };
StmtList:
    StmtList Stmt {
        $$ = $1;
        $$.build_AST($2);
    } | Stmt {
        $$ = Token("BlockStmt");
        $$.build_AST($1);
    };
Stmt:
    LocalDeclDef
      | BlockStmt
      | Expr PUNCTUATOR_SEMICOLON
      | KEYWORD_RETURN Expr PUNCTUATOR_SEMICOLON {
        $$ = Token("ReturnStmt");
        $$.build_AST($2);
    } | KEYWORD_CONTINUE Expr PUNCTUATOR_SEMICOLON {
        $$ = Token("ContinueStmt");
        $$.build_AST($2);
    } | KEYWORD_BREAK Expr PUNCTUATOR_SEMICOLON {
        $$ = Token("BreakStmt");
        $$.build_AST($2);
    } | KEYWORD_RETURN PUNCTUATOR_SEMICOLON {
        $$ = Token("ReturnStmt");
    } | KEYWORD_CONTINUE PUNCTUATOR_SEMICOLON {
        $$ = Token("ContinueStmt");
    } | KEYWORD_BREAK PUNCTUATOR_SEMICOLON {
        $$ = Token("BreakStmt");
    } | KEYWORD_IF PUNCTUATOR_PARENTHESIS_LEFT Expr PUNCTUATOR_PARENTHESIS_RIGHT Stmt %prec IF_END {
        $$ = Token("IfStmt");
        $$.build_AST($3);
        $$.build_AST($5);
    } | KEYWORD_IF PUNCTUATOR_PARENTHESIS_LEFT Expr PUNCTUATOR_PARENTHESIS_RIGHT Stmt KEYWORD_ELSE Stmt {
        $$ = Token("IfStmt");
        $$.build_AST($3);
        $$.build_AST($5);
        Token elseStmt("ElseStmt");
        elseStmt.build_AST($7);
        $$.build_AST(elseStmt);
    } | KEYWORD_WHILE PUNCTUATOR_PARENTHESIS_LEFT Expr PUNCTUATOR_PARENTHESIS_RIGHT Stmt {
        $$ = Token("WhileStmt");
        $$.build_AST($3);
        $$.build_AST($5);
    } | KEYWORD_DO Stmt KEYWORD_WHILE PUNCTUATOR_PARENTHESIS_LEFT Expr PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_SEMICOLON {
        $$ = Token("DoWhileStmt");
        $$.build_AST($2);
        $$.build_AST($5);
    } | KEYWORD_FOR PUNCTUATOR_PARENTHESIS_LEFT LocalDeclDef Expr PUNCTUATOR_SEMICOLON Expr PUNCTUATOR_PARENTHESIS_RIGHT Stmt {
        $$ = Token("ForStmt");
        $$.build_AST($3);
        $$.build_AST($4);
        $$.build_AST($6);
        $$.build_AST($8);
    } | KEYWORD_FOR PUNCTUATOR_PARENTHESIS_LEFT Expr PUNCTUATOR_SEMICOLON Expr PUNCTUATOR_SEMICOLON Expr PUNCTUATOR_PARENTHESIS_RIGHT Stmt {
        $$ = Token("ForStmt");
        $$.build_AST($3);
        $$.build_AST($5);
        $$.build_AST($7);
        $$.build_AST($9);
    } | KEYWORD_SWITCH PUNCTUATOR_PARENTHESIS_LEFT Expr PUNCTUATOR_PARENTHESIS_RIGHT PUNCTUATOR_BRACE_LEFT SwitchBodyStmt PUNCTUATOR_BRACE_RIGHT {
        $$ = Token("SwitchStmt");
        $$.build_AST($3);
        $$.build_AST($6);
    } | error PUNCTUATOR_SEMICOLON {
        $$ = Token("ErrorStmt");
        yyerrok;
    };
SwitchBodyStmt:
    CaseStmtList DefaultStmt {
        $$ = $1;
        $$.build_AST($2);
    } | CaseStmtList
      | DefaultStmt {
        $$ = Token("SwitchBodyStmt");
        $$.build_AST($1);
    } | {
        $$ = Token("SwitchBodyStmt");
    };
CaseStmtList:
    CaseStmtList CaseStmt {
        $$ = $1;
        $$.build_AST($2);
    } | CaseStmt {
        $$ = Token("SwitchBodyStmt");
        $$.build_AST($1);
    };
CaseStmt:
    KEYWORD_CASE Expr PUNCTUATOR_COLON StmtList {
        $$ = Token("CaseStmt");
        $$.build_AST($2);
        $$.build_AST($4);
    } | KEYWORD_CASE Expr PUNCTUATOR_COLON {
        $$ = Token("CaseStmt");
        $$.build_AST($2);
    };
DefaultStmt:
    KEYWORD_DEFAULT PUNCTUATOR_COLON StmtList {
        $$ = Token("DefaultStmt");
        $$.build_AST($3);
    } | KEYWORD_DEFAULT PUNCTUATOR_COLON {
        $$ = Token("DefaultStmt");
    };
LocalDeclDef:
    Type VarList PUNCTUATOR_SEMICOLON {
        $$ = Token("LocalVarDecl");
        $$.build_AST($1);
        $$.build_AST($2);
    };
Expr:
    Assignable | Literal | CallExpr | AssignmentExpr
    | Assignable OPERATOR_COMPOUND_ASSIGNMENT Expr {
        $$ = Token("CompoundAssignmentExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_QUESTION Expr PUNCTUATOR_COLON Expr {
        $$ = Token("TernaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", "?"));
        $$.build_AST($3);
        $$.build_AST(Token("Operator", ":"));
        $$.build_AST($5);
    } | Expr OPERATOR_LOGICAL_AND Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_LOGICAL_OR Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_BITWISE_AND Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_BITWISE_OR Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_BITWISE_XOR Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_SHIFT_RIGHT Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_SHIFT_LEFT Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_RELATIONAL Expr {
        $$ = Token("RelationalExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_PLUS Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_MINUS Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_MULTIPLY Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_DIVIDE Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | Expr OPERATOR_MODULUS Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
        $$.build_AST($3);
    } | PUNCTUATOR_PARENTHESIS_LEFT Expr PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = Token("ParenthesisExpr");
        $$.build_AST($2);
    } | OPERATOR_PLUS Expr %prec OPERATOR_UNARY_PLUS {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1));
        $$.build_AST($2);
    } | OPERATOR_MINUS Expr %prec OPERATOR_UNARY_MINUS {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1));
        $$.build_AST($2);
    } | OPERATOR_LOGICAL_NOT Expr {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1));
        $$.build_AST($2);
    } | OPERATOR_BITWISE_NOT Expr {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1));
        $$.build_AST($2);
    } | OPERATOR_INCREMENT Expr {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1));
        $$.build_AST($2);
    } | OPERATOR_DECREMENT Expr {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1));
        $$.build_AST($2);
    } | Expr OPERATOR_INCREMENT {
        $$ = Token("PostfixExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
    } | Expr OPERATOR_DECREMENT {
        $$ = Token("PostfixExpr");
        $$.build_AST($1);
        $$.build_AST(Token("Operator", $2));
    };
Literal:
    LITERAL_INTEGER {
        $$ = Token("IntegerLiteral", $1);
    } | LITERAL_FLOAT {
        $$ = Token("FloatLiteral", $1);
    } | LITERAL_CHAR {
        $$ = Token("CharLiteral", $1);
    } | LITERAL_STRING {
        $$ = Token("StringLiteral", $1);
    };
ArrayLiteral:
    PUNCTUATOR_BRACE_LEFT ArrayItemList PUNCTUATOR_BRACE_RIGHT {
        $$ = $2;
    };
ArrayItemList:
    ArrayItemList PUNCTUATOR_COMMA ArrayItem {
        $$ = $1;
        $$.build_AST($3);
    } | ArrayItem {
        $$ = Token("ArrayLiteral");
        $$.build_AST($1);
    } | {
        $$ = Token("ArrayLiteral");
    };
ArrayItem:
    Expr | ArrayLiteral;
CallExpr:
    CallExpr PUNCTUATOR_PARENTHESIS_LEFT ArgumentList PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = $1;
        $$.build_AST($3);
    } | Assignable PUNCTUATOR_PARENTHESIS_LEFT ArgumentList PUNCTUATOR_PARENTHESIS_RIGHT {
        $$ = Token("CallExpr");
        $$.build_AST($1);
        $$.build_AST($3);
    };
ArgumentList:
    ArgumentList PUNCTUATOR_COMMA Expr {
        $$ = $1;
        $$.build_AST($3);
    } | Expr {
        $$ = Token("ArgumentList");
        $$.build_AST($1);
    } | {
        $$ = Token("ArgumentList");
    };
IndexExpr:
    IndexExpr PUNCTUATOR_BRACKET_LEFT Index PUNCTUATOR_BRACKET_RIGHT {
        $$ = $1;
        $$.build_AST($3);
    } | Identifier PUNCTUATOR_BRACKET_LEFT Index PUNCTUATOR_BRACKET_RIGHT {
        $$ = Token("IndexExpr");
        $$.build_AST($1);
        $$.build_AST($3);
    };
Index:
    Expr {
        $$ = Token("Index");
        $$.build_AST($1);
    } | {
        $$ = Token("Index");
    };
%%

void Parser::error(const location &loc, const string &message) {
     cout << RED << "Error: " << message << endl << YELLOW << "Location: " << loc << RESET_COLOR << endl;
}
