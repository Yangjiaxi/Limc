# Grammar

```java
Program ->
    // < a.file
    | GlobalDeclDefList

GlobalDeclDefList ->
    // GlobalDeclDef {GlobalDeclDef}
    | GlobalDeclDefList GlobalDeclDef
    // GlobalDeclDef
    | GlobalDeclDef

GlobalDeclDef ->
    // int a, b, ...;
    | Type VarList DELIM_SEMICOLON
    // int fn(char b, ...) {stmts}
    | Type Assignable DELIM_PARENTHESIS_LEFT ParamList DELIM_PARENTHESIS_RIGHT BlockStmt
    // int fn(char a, ...);
    | Type Assignable DELIM_PARENTHESIS_LEFT ParamList DELIM_PARENTHESIS_RIGHT DELIM_SEMICOLON
    | [error] DELIM_SEMICOLON

Type ->
    // int
    | KW_TYPE

Assignable ->
    // name
    | Identifier
    // array_name[12+5][strlen("")]
    | IndexExpr

Identifier ->
    // Using Regex
    IDENTIFIER

VarList ->
    | Assignable
    | AssignmentExpr
    // Left-Recursion Eliminate
    | VarList DELIM_COMMA Assignable
    | VarList DELIM_COMMA AssignmentExpr

AssignmentExpr ->
    // a = 12 + 5
    | Assignable OP_ASSIGNMENT Expr
    // array_name = [1, 2, 3, 4]
    | Assignable OP_ASSIGNMENT ArrayLiteral

ParamList ->
    // LRE
    // 1, 2, ...
    | ParamList DELIM_COMMA Param
    | Param
    | [eps]

Param ->
    // int param_1
    Type Assignable

BlockStmt ->
    // {stmt1 stmt2 ...}
    | DELIM_BRACE_LEFT StmtList DELIM_BRACE_RIGHT
    // { }
    | DELIM_BRACE_LEFT DELIM_BRACE_RIGHT
    | [error] DELIM_BRACE_RIGHT

StmtList ->
    // LRE
    | StmtList Stmt
    // xxxx
    | Stmt

Stmt ->
    | LocalDeclDef
    | BlockStmt
    // xxx ;
    | Expr DELIM_SEMICOLON
    // return [Expr] ;
    | KW_RETURN Expr DELIM_SEMICOLON
    // return ;
    | KW_RETURN DELIM_SEMICOLON
    // continue ;
    | KW_CONTINUE DELIM_SEMICOLON
    // break ;
    | KW_BREAK DELIM_SEMICOLON
    // if ([Expr]) [Stmt]
    | KW_IF DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt %prec IF_END
    // if ([Expr]) [Stmt] else [Stmt]
    | KW_IF DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt KW_ELSE Stmt
    // while ([Expr]) [Stmt]
    | KW_WHILE DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT Stmt
    // do [Stmt] while ([Expr])
    | KW_DO Stmt KW_WHILE DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT DELIM_SEMICOLON
    // for ([LocalDeclDef] ; [Expr] ; [Expr]) [Stmt]
    | KW_FOR DELIM_PARENTHESIS_LEFT LocalDeclDef Expr DELIM_SEMICOLON Expr DELIM_PARENTHESIS_RIGHT Stmt
    // for ([Expr] ; [Expr] ; [Expr]) [Stmt]
    | KW_FOR DELIM_PARENTHESIS_LEFT Expr DELIM_SEMICOLON Expr DELIM_SEMICOLON Expr DELIM_PARENTHESIS_RIGHT Stmt
    // switch ([Expr]) { [SwitchBodyStmt] }
    | KW_SWITCH DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT DELIM_BRACE_LEFT SwitchBodyStmt DELIM_BRACE_RIGHT
    | [error] DELIM_SEMICOLON

SwitchBodyStmt ->
    // LRE
    // {[Case]...}, {[Default]}
    | CaseStmtList DefaultStmt
    | CaseStmtList
    | DefaultStmt
    | [eps]

CaseStmtList ->
    // LRE
    | CaseStmtList CaseStmt
    | CaseStmt

CaseStmt ->
    // case Expr : [StmtList]
    | KW_CASE Expr DELIM_COLON StmtList
    // case Expr :
    | KW_CASE Expr DELIM_COLON

DefaultStmt ->
    // default : [StmtList]
    | KW_DEFAULT DELIM_COLON StmtList
    // default :
    | KW_DEFAULT DELIM_COLON

LocalDeclDef ->
    // [Type] a, b, c...;
    | Type VarList DELIM_SEMICOLON

Expr ->
    | Assignable
    // 12, 'a', 'string', [1, 2, 3, ...]
    | Literal
    // fn(12), fn(12)('a')("abc")
    | CallExpr
    | AssignmentExpr
    // var += [Expr]
    | Assignable OP_COMPOUND_ASSIGNMENT Expr
    // b_var ? [Expr] : [Expr]
    | Expr DELIM_QUESTION Expr DELIM_COLON Expr
    // a && b
    | Expr OP_LOGICAL_AND Expr
    // a || b
    | Expr OP_LOGICAL_OR Expr
    // a & b
    | Expr OP_BITWISE_AND Expr
    // a | b
    | Expr OP_BITWISE_OR Expr
    // a ^ b
    | Expr OP_BITWISE_XOR Expr
    // a << 12
    | Expr OP_SHIFT_RIGHT Expr
    // b >> 12
    | Expr OP_SHIFT_LEFT Expr
    // a ( > | >= | != | ...) b
    | Expr OP_RELATIONAL Expr
    // a + b
    | Expr OP_PLUS Expr
    // a - b
    | Expr OP_MINUS Expr
    // a * b
    | Expr OP_MULTIPLY Expr
    // a / b
    | Expr OP_DIVIDE Expr
    // a % b
    | Expr OP_MODULUS Expr
    // ( [Expr] )
    | DELIM_PARENTHESIS_LEFT Expr DELIM_PARENTHESIS_RIGHT
    // + [Expr] => `Positive of Expr`
    | OP_PLUS Expr %prec OP_UNARY_PLUS
    // - [Expr] => `Negative of Expr`
    | OP_MINUS Expr %prec OP_UNARY_MINUS
    // !b
    | OP_LOGICAL_NOT Expr
    // ~b
    | OP_BITWISE_NOT Expr
    // ++a
    | OP_INCREMENT Expr
    // --b
    | OP_DECREMENT Expr
    // a++
    | Expr OP_INCREMENT
    // b--
    | Expr OP_DECREMENT

Literal ->
    // 12L
    | LITERAL_INTEGER
    // 12.412313f
    | LITERAL_FLOAT
    // 'a'
    | LITERAL_CHAR
    // "HELLO WORLD"
    | LITERAL_STRING

ArrayLiteral ->
    // [1, 2, ...]
    | DELIM_BRACE_LEFT ArrayItemList DELIM_BRACE_RIGHT

ArrayItemList ->
    // LRE
    // 1, 2, ...
    | ArrayItemList DELIM_COMMA ArrayItem
    | ArrayItem
    | [eps]

ArrayItem ->
    | Expr
    | ArrayLiteral

CallExpr ->
    // High-order Function
    // fn(...)(...)(...)...
    | CallExpr DELIM_PARENTHESIS_LEFT ArgumentList DELIM_PARENTHESIS_RIGHT
    // fn (a, b, ...) || fn[5] (a, b, ...)
    | Assignable DELIM_PARENTHESIS_LEFT ArgumentList DELIM_PARENTHESIS_RIGHT

ArgumentList ->
    // a, b, ...
    | ArgumentList DELIM_COMMA Expr
    | Expr
    | [eps]

IndexExpr ->
    // LRE
    // a[5][[Expr]]
    | IndexExpr DELIM_BRACKET_LEFT Index DELIM_BRACKET_RIGHT
    // a[12]
    | Identifier DELIM_BRACKET_LEFT Index DELIM_BRACKET_RIGHT

Index ->
    | Expr
    | [eps]
```

```java
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
    | ErrorStmt
    | BlockStmt
    | ReturnStmt
    | ContinueStmt
    | BreakStmt
    | ContinueStmt
    | IfStmt
    | ElseStmt
    | WhileStmt
    | DoWhileStmt
    | ForStmt
    | SwitchStmt
    | SwitchBodyStmt
    | CaseStmt
    | DefaultStmt

Decl ->
    | GlobalVarDecl
    | FunctionDecl
    | ParamDecl
    | LocalVarDecl

List ->
    | GlobalDeclDefList
    | VarList
    | ParamList
    | ArgumentList
```
