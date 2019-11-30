# Grammar

## Note

- Pointer is not same as std C
  - C: `int *a, b;`
    - `a` : pointer to `int`
    - `b` : `int`
  - Limc: `int* a, b;`
    - `a` : pointer to `int`
    - `b` : pointer to `int`
  - Explain: DO_NOT_USE_int_*a,_b;__OR_YOU_WILL_BE_FIRED

## Check List

### Other

- [x] Program
- [ ] Type
- [x] Pointer
- [x] Identifier
- [ ] Operator
- [ ] Index

### Def

- [x] FuncDef
- [x] VarDef

### Decl

- [x] GlobalVarDecl
- [ ] FuncDecl
- [ ] VarDecl
- [ ] ParamDecl
- [x] LocalVarDecl

### List

- [ ] VarList
- [ ] ParamList
- [ ] ArgumentList

### Expr

- [x] AssignmentExpr
- [x] CompoundAssignmentExpr
- [x] TernaryExpr
- [x] BinaryExpr
- [x] RelationalExpr
- [x] ParenthesisExpr
- [x] PrefixExpr
- [x] PostfixExpr
- [x] CallExpr
- [x] IndexExpr

### Stmt

- [x] BlockStmt
- [x] ReturnStmt
- [x] ContinueStmt
- [x] BreakStmt
- [ ] IfStmt
- [ ] ElseStmt
- [x] WhileStmt
- [x] DoWhileStmt
- [x] ForStmt
- [x] SwitchStmt
- [ ] SwitchBodyStmt
- [ ] CaseStmt
- [ ] DefaultStmt

### Literal

- [x] IntegerLiteral
- [x] FloatLiteral
- [x] CharLiteral
- [x] StringLiteral
- [x] ArrayLiteral

### Error

- [ ] ErrorStmt

## AST Building

```java
Program:
    | GlobalDeclDefList

GlobalDeclDefList:
    | GlobalDeclDefList GlobalDeclDef
    | GlobalDeclDef

GlobalDeclDef:
    | Type VarList DELIM_SEMICOLON
    | Type Assignable DELIM_PARENTHESIS_LEFT ParamList DELIM_PARENTHESIS_RIGHT BlockStmt
    | Type Assignable DELIM_PARENTHESIS_LEFT ParamList DELIM_PARENTHESIS_RIGHT DELIM_SEMICOLON
    | error DELIM_SEMICOLON

Type:
    | KW_TYPE
    | Type OP_MULTIPLY

Assignable:
    | Identifier
    | IndexExpr

Identifier:
    | IDENTIFIER

VarList:
    | Assignable
    | AssignmentExpr
    | VarList DELIM_COMMA Assignable
    | VarList DELIM_COMMA AssignmentExpr

AssignmentExpr:
    | Assignable OP_ASSIGNMENT Expr
    | Assignable OP_ASSIGNMENT ArrayLiteral

ParamList:
    | ParamList DELIM_COMMA Param
    | Param
    | %empty

Param:
    | Type Assignable

BlockStmt:
    | DELIM_BRACE_LEFT StmtList DELIM_BRACE_RIGHT
    | DELIM_BRACE_LEFT DELIM_BRACE_RIGHT
    | error DELIM_BRACE_RIGHT

StmtList:
    | StmtList Stmt
    | Stmt

Stmt:
    | LocalDeclDef
    | BlockStmt
    | Expr DELIM_SEMICOLON
    | KW_RETURN Expr DELIM_SEMICOLON
    | KW_CONTINUE Expr DELIM_SEMICOLON
    | KW_BREAK Expr DELIM_SEMICOLON
    | KW_RETURN DELIM_SEMICOLON
    | KW_CONTINUE DELIM_SEMICOLON
    | KW_BREAK DELIM_SEMICOLON
    | KW_IF DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt
    | KW_IF DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt KW_ELSE Stmt
    | KW_WHILE DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt
    | KW_DO Stmt KW_WHILE DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT DELIM_SEMICOLON
    | KW_FOR DELIM_PARENTHESIS_LEFT LocalDeclDef Expr DELIM_SEMICOLON Expr DELIM_PARENTHESIS_RIGHT Stmt
    | KW_FOR DELIM_PARENTHESIS_LEFT Expr DELIM_SEMICOLON Expr DELIM_SEMICOLON Expr DELIM_PARENTHESIS_RIGHT Stmt
    | KW_SWITCH DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT DELIM_BRACE_LEFT SwitchBodyStmt DELIM_BRACE_RIGHT
    | error DELIM_SEMICOLON

SwitchBodyStmt:
    | CaseStmtList DefaultStmt
    | CaseStmtList
    | DefaultStmt
    | %empty

CaseStmtList:
    | CaseStmtList CaseStmt
    | CaseStmt

CaseStmt:
    | KW_CASE Expr DELIM_COLON StmtList
    | KW_CASE Expr DELIM_COLON

DefaultStmt:
    | KW_DEFAULT DELIM_COLON StmtList
    | KW_DEFAULT DELIM_COLON

LocalDeclDef:
    | Type VarList DELIM_SEMICOLON

Expr:
    | Assignable
    | Literal
    | CallExpr
    | AssignmentExpr
    | Assignable OP_COMPOUND_ASSIGNMENT Expr
    | Expr DELIM_QUESTION Expr DELIM_COLON Expr
    | Expr OP_LOGICAL_AND Expr
    | Expr OP_LOGICAL_OR Expr
    | Expr OP_BITWISE_AND Expr
    | Expr OP_BITWISE_OR Expr
    | Expr OP_BITWISE_XOR Expr
    | Expr OP_SHIFT_RIGHT Expr
    | Expr OP_SHIFT_LEFT Expr
    | Expr OP_RELATIONAL Expr
    | Expr OP_PLUS Expr
    | Expr OP_MINUS Expr
    | Expr OP_MULTIPLY Expr
    | Expr OP_DIVIDE Expr
    | Expr OP_MODULUS Expr
    | DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT
    | OP_PLUS Expr
    | OP_MINUS Expr
    | OP_MULTIPLY Identifier
    | OP_BITWISE_AND Identifier
    | OP_LOGICAL_NOT Expr
    | OP_BITWISE_NOT Expr
    | OP_INCREMENT Expr
    | OP_DECREMENT Expr
    | Expr OP_INCREMENT
    | Expr OP_DECREMENT

Literal:
    | LITERAL_INTEGER
    | LITERAL_FLOAT
    | LITERAL_CHAR
    | LITERAL_STRING

ArrayLiteral:
    | DELIM_BRACE_LEFT ArrayItemList DELIM_BRACE_RIGHT

ArrayItemList:
    | ArrayItemList DELIM_COMMA ArrayItem
    | ArrayItem
    | %empty

ArrayItem:
    | Expr
    | ArrayLiteral

CallExpr:
    | CallExpr DELIM_PARENTHESIS_LEFT ArgumentList DELIM_PARENTHESIS_RIGHT
    | Assignable DELIM_PARENTHESIS_LEFT ArgumentList DELIM_PARENTHESIS_RIGHT

ArgumentList:
    | ArgumentList DELIM_COMMA Expr
    | Expr
    | %empty

IndexExpr:
    | IndexExpr DELIM_BRACKET_LEFT Index DELIM_BRACKET_RIGHT
    | Identifier DELIM_BRACKET_LEFT Index DELIM_BRACKET_RIGHT

Index:
    | Expr
    | %empty
```

### Name of AST Node

```Java
Error ->
    | ErrorStmt

Def ->
    | FuncDef
    | VarDef

Decl ->
    | GlobalVarDecl
    | FuncDecl
    | VarDecl
    | ParamDecl
    | LocalVarDecl

List ->
    | VarList
    | ParamList
    | ArgumentList

Expr ->
    | AssignmentExpr
    | CompoundAssignmentExpr
    | TernaryExpr
    | BinaryExpr
    | RelationalExpr
    | ParenthesisExpr
    | PrefixExpr
    | PostfixExpr
    | CallExpr
    | IndexExpr

Stmt ->
    | BlockStmt
    | ReturnStmt
    | ContinueStmt
    | BreakStmt
    | IfStmt
    | ElseStmt
    | WhileStmt
    | DoWhileStmt
    | ForStmt
    | SwitchStmt
    | SwitchBodyStmt
    | CaseStmt
    | DefaultStmt

Literal ->
    | IntegerLiteral
    | FloatLiteral
    | CharLiteral
    | StringLiteral
    | ArrayLiteral

Program

Type

Pointer

Identifier

Operator

Index
```
