%skeleton "lalr1.cc"
%require "3.4.2"
%defines
%define api.namespace { Limc }
%define api.parser.class { Parser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
    #include "token.h"

    namespace Limc {
        class Scanner;
        class Driver;
    }
}

%code top {
    #include "driver.h"
    #include "color.h"
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
%token DELIM_PARENTHESIS_LEFT DELIM_PARENTHESIS_RIGHT DELIM_BRACE_LEFT DELIM_BRACE_RIGHT DELIM_BRACKET_LEFT DELIM_BRACKET_RIGHT DELIM_SEMICOLON DELIM_COMMA DELIM_QUESTION DELIM_COLON
%token KW_FOR KW_IF KW_ELSE KW_WHILE KW_DO KW_RETURN KW_BREAK KW_CONTINUE KW_SWITCH KW_DEFAULT KW_CASE

%token <string> IDENTIFIER KW_TYPE LITERAL_FLOAT LITERAL_INTEGER LITERAL_CHAR LITERAL_STRING
%token <string> OP_SHIFT_RIGHT OP_SHIFT_LEFT OP_BITWISE_AND OP_BITWISE_OR OP_BITWISE_XOR OP_BITWISE_NOT
%token <string> OP_LOGICAL_NOT OP_LOGICAL_AND OP_LOGICAL_OR
%token <string> OP_COMPOUND_ASSIGNMENT OP_RELATIONAL OP_ASSIGNMENT
%token <string> OP_PLUS OP_MINUS OP_MULTIPLY OP_DIVIDE OP_MODULUS
%token <string> OP_INCREMENT OP_DECREMENT

%left DELIM_QUESTION DELIM_COLON
%left OP_ASSIGNMENT OP_COMPOUND_ASSIGNMENT
%left OP_LOGICAL_OR
%left OP_LOGICAL_AND
%left OP_BITWISE_OR
%left OP_BITWISE_XOR
%left OP_BITWISE_AND
%left OP_RELATIONAL
%left OP_SHIFT_RIGHT OP_SHIFT_LEFT
%left OP_PLUS OP_MINUS
%left OP_MULTIPLY OP_DIVIDE OP_MODULUS

%right OP_LOGICAL_NOT OP_BITWISE_NOT

%nonassoc IF_END
%nonassoc KW_ELSE
%nonassoc OP_UNARY_MINUS OP_UNARY_PLUS
%nonassoc OP_INCREMENT OP_DECREMENT

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
    Type VarList DELIM_SEMICOLON {
        $$ = Token("GlobalVarDecl");
        $$.build_AST($1)
          .build_AST($2);
    } | Type Assignable DELIM_PARENTHESIS_LEFT ParamList DELIM_PARENTHESIS_RIGHT BlockStmt {
        $$ = Token("FunctionDef");
        $$.build_AST($1)
          .build_AST($2)
          .build_AST($4)
          .build_AST($6);
    } | Type Assignable DELIM_PARENTHESIS_LEFT ParamList DELIM_PARENTHESIS_RIGHT DELIM_SEMICOLON {
        $$ = Token("FunctionDecl");
        $$.build_AST($1)
          .build_AST($2)
          .build_AST($4);
    } | error DELIM_SEMICOLON {
        $$ = Token("ErrorStmt");
        yyerrok;
    };
Type:
    KW_TYPE {
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
    } | VarList DELIM_COMMA Assignable {
        $$ = $1;
        $$.build_AST($3);
    } | VarList DELIM_COMMA AssignmentExpr {
        $$ = $1;
        $$.build_AST($3);
    };
AssignmentExpr:
    Assignable OP_ASSIGNMENT Expr {
        $$ = Token("AssignmentExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Assignable OP_ASSIGNMENT ArrayLiteral {
        $$ = Token("AssignmentExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    };
ParamList:
    ParamList DELIM_COMMA Param {
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
        $$.build_AST($1)
          .build_AST($2);
    };
BlockStmt:
    DELIM_BRACE_LEFT StmtList DELIM_BRACE_RIGHT {
        $$ = $2;
    } | DELIM_BRACE_LEFT DELIM_BRACE_RIGHT {
        $$ = Token("BlockStmt");
    } | error DELIM_BRACE_RIGHT {
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
      | Expr DELIM_SEMICOLON
      | KW_RETURN Expr DELIM_SEMICOLON {
        $$ = Token("ReturnStmt");
        $$.build_AST($2);
    } | KW_CONTINUE Expr DELIM_SEMICOLON {
        $$ = Token("ContinueStmt");
        $$.build_AST($2);
    } | KW_BREAK Expr DELIM_SEMICOLON {
        $$ = Token("BreakStmt");
        $$.build_AST($2);
    } | KW_RETURN DELIM_SEMICOLON {
        $$ = Token("ReturnStmt");
    } | KW_CONTINUE DELIM_SEMICOLON {
        $$ = Token("ContinueStmt");
    } | KW_BREAK DELIM_SEMICOLON {
        $$ = Token("BreakStmt");
    } | KW_IF DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt %prec IF_END {
        $$ = Token("IfStmt");
        $$.build_AST($3)
          .build_AST($5);
    } | KW_IF DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt KW_ELSE Stmt {
        $$ = Token("IfStmt");
        $$.build_AST($3)
          .build_AST($5);
        Token else_stmt("ElseStmt");
        else_stmt.build_AST($7);
        $$.build_AST(else_stmt);
    } | KW_WHILE DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt {
        $$ = Token("WhileStmt");
        $$.build_AST($3)
          .build_AST($5);
    } | KW_DO Stmt KW_WHILE DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT DELIM_SEMICOLON {
        $$ = Token("DoWhileStmt");
        $$.build_AST($2)
          .build_AST($5);
    } | KW_FOR DELIM_PARENTHESIS_LEFT LocalDeclDef Expr DELIM_SEMICOLON Expr DELIM_PARENTHESIS_RIGHT Stmt {
        $$ = Token("ForStmt");
        $$.build_AST($3)
          .build_AST($4)
          .build_AST($6)
          .build_AST($8);
    } | KW_FOR DELIM_PARENTHESIS_LEFT Expr DELIM_SEMICOLON Expr DELIM_SEMICOLON Expr DELIM_PARENTHESIS_RIGHT Stmt {
        $$ = Token("ForStmt");
        $$.build_AST($3)
          .build_AST($5)
          .build_AST($7)
          .build_AST($9);
    } | KW_SWITCH DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT DELIM_BRACE_LEFT SwitchBodyStmt DELIM_BRACE_RIGHT {
        $$ = Token("SwitchStmt");
        $$.build_AST($3)
          .build_AST($6);
    } | error DELIM_SEMICOLON {
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
    KW_CASE Expr DELIM_COLON StmtList {
        $$ = Token("CaseStmt");
        $$.build_AST($2)
          .build_AST($4);
    } | KW_CASE Expr DELIM_COLON {
        $$ = Token("CaseStmt");
        $$.build_AST($2);
    };
DefaultStmt:
    KW_DEFAULT DELIM_COLON StmtList {
        $$ = Token("DefaultStmt");
        $$.build_AST($3);
    } | KW_DEFAULT DELIM_COLON {
        $$ = Token("DefaultStmt");
    };
LocalDeclDef:
    Type VarList DELIM_SEMICOLON {
        $$ = Token("LocalVarDecl");
        $$.build_AST($1)
          .build_AST($2);
    };
Expr:
    Assignable | Literal | CallExpr | AssignmentExpr
    | Assignable OP_COMPOUND_ASSIGNMENT Expr {
        $$ = Token("CompoundAssignmentExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr DELIM_QUESTION Expr DELIM_COLON Expr {
        $$ = Token("TernaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", "?"))
          .build_AST($3)
          .build_AST(Token("Operator", ":"))
          .build_AST($5);
    } | Expr OP_LOGICAL_AND Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_LOGICAL_OR Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_BITWISE_AND Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_BITWISE_OR Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_BITWISE_XOR Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_SHIFT_RIGHT Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_SHIFT_LEFT Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_RELATIONAL Expr {
        $$ = Token("RelationalExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_PLUS Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_MINUS Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_MULTIPLY Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_DIVIDE Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | Expr OP_MODULUS Expr {
        $$ = Token("BinaryExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2))
          .build_AST($3);
    } | DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT {
        $$ = Token("ParenthesisExpr");
        $$.build_AST($2);
    } | OP_PLUS Expr %prec OP_UNARY_PLUS {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1))
          .build_AST($2);
    } | OP_MINUS Expr %prec OP_UNARY_MINUS {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1))
          .build_AST($2);
    } | OP_LOGICAL_NOT Expr {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1))
          .build_AST($2);
    } | OP_BITWISE_NOT Expr {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1))
          .build_AST($2);
    } | OP_INCREMENT Expr {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1))
          .build_AST($2);
    } | OP_DECREMENT Expr {
        $$ = Token("PrefixExpr");
        $$.build_AST(Token("Operator", $1))
          .build_AST($2);
    } | Expr OP_INCREMENT {
        $$ = Token("PostfixExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2));
    } | Expr OP_DECREMENT {
        $$ = Token("PostfixExpr");
        $$.build_AST($1)
          .build_AST(Token("Operator", $2));
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
    DELIM_BRACE_LEFT ArrayItemList DELIM_BRACE_RIGHT {
        $$ = $2;
    };
ArrayItemList:
    ArrayItemList DELIM_COMMA ArrayItem {
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
    CallExpr DELIM_PARENTHESIS_LEFT ArgumentList DELIM_PARENTHESIS_RIGHT {
        $$ = $1;
        $$.build_AST($3);
    } | Assignable DELIM_PARENTHESIS_LEFT ArgumentList DELIM_PARENTHESIS_RIGHT {
        $$ = Token("CallExpr");
        $$.build_AST($1)
          .build_AST($3);
    };
ArgumentList:
    ArgumentList DELIM_COMMA Expr {
        $$ = $1;
        $$.build_AST($3);
    } | Expr {
        $$ = Token("ArgumentList");
        $$.build_AST($1);
    } | {
        $$ = Token("ArgumentList");
    };
IndexExpr:
    IndexExpr DELIM_BRACKET_LEFT Index DELIM_BRACKET_RIGHT {
        $$ = $1;
        $$.build_AST($3);
    } | Identifier DELIM_BRACKET_LEFT Index DELIM_BRACKET_RIGHT {
        $$ = Token("IndexExpr");
        $$.build_AST($1)
          .build_AST($3);
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
