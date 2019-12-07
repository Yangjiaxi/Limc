# Limc

Limc Is a Minimal Compiler

## Token Kinds


### Part.1 Token without Type

```

// 程序，AST的根，包含所有
Program

// 全局*变量*声明
// 需要大小作为相对全局基址的偏移
GlobalVarDecl
  - 0: <Type>
  - 1: <VarList>

// 函数定义
FuncDef
  - 0: `return` <Type>
  - 1: `name` <Identifier>
  - 2: ParamList
    - {ParamDecl}
  - 3: BlockStmt

// 函数声明
FuncDecl
  - 0: `return` Type
  - 1: `name` <Identifier>
  - 2: ParamList
    - {ParamDecl}

// 普通类型，由关键字给出
Type
  - 0: <Type>

// 结构体类型
Struct
  - 0: StructList
    - {StructItemDecl}

// 变量列表，声明+定义都可以有
VarList
  - {VarDecl}
  - {VarDef}

// 由花括号包围的多条语句，可为空块
BlockStmt

// 一般来说用于函数末尾，返回值`<Expr>`类型需要匹配所在函数的需求类型
ReturnStmt
  - 0: <Expr>

// 循环中的continue与break关键字
ContinueStmt | BreakStmt
  IN:
    | WhileStmt
    | DoWhileStmt
    | ForStmt

// while循环，真假表达式+块
WhileStmt
  - 0: <Expr>
  - 1: <Stmt>

// do-while循环
DoWhileStmt
  - 0: <Stmt>
  - 1: <Expr>

// for循环
ForStmt
  - 0: <LocalDeclDef> | <Expr>
  - 1: <Expr>
  - 2: <Expr>
  - 3: <Stmt>

// if判断，条件，为真时语句，为假时语句
IfStmt
  - 0: `cond` <Expr>
  - 1: `then` <Stmt>
  - [2]: `else` <Stmt>

// 局部变量声明，只可能成为`stmt`或在`for`中
// 需要得到变量的大小，作为相对基址的偏移
// 一个块内的所有变量大小求和，得到这个块需要的大小
// 这个块的大小便是分配局部空间的依据
LocalVarDecl:
// 只在块中出现
 IN:
    | Stmt

// 传递给被调函数的参数列表，需要匹配数量与类型
ArgumentList:
  - {Expr}

// 数组下标索引，可以多重
Index:
// 使用时需要判断是不是`整数`类型即可
  IN:
    | IndexExpr

```

### Part.2 Token with Type

总之，一切`Expr`都是具有类型的，大块的`Expr`的类型可以从小块的`Expr`拼起来

比如`cond ? then : els;`

`cond`类型为`int`，`then`与`els`在具有相同类型时，该表达式具有`then`的类型

```

标识符，在使用时需要查看符号表，返回符号表中相应条目的类型
Identifier

数组元素索引表达式，获得被索引的元素类型后进行Array的解套娃
IndexExpr

成员访问，需要判断是否为struct，成员是否存在，返回成员的类型，给出相对目标结构体的偏移
MemberExpr

聚合字面量，数组或结构体初始化，不具有Type，但是可以通过比较后赋值给Struct或Array类型
AggrLiteral

整数字面量
IntegerLiteral

浮点数字面量
FloatLiteral

字符字面量
CharLiteral

/// TODO
字符串字面量
StringLiteral

调用函数的语句，需要检查类型、数量，返回对应函数的返回值的类型
CallExpr

赋值语句，返回被赋值的左值的类型
AssignmentExpr

结合赋值，先运算后赋值，同上
CompoundAssignmentExpr

括号表达式，返回内部类型
ParenthesisExpr

前缀表达式
PrefixExpr

后缀表达式
PostfixExpr

二元(逻辑 | 算数)表达式
BinaryExpr

关系表达式，返回int 0->false, 1->true
RelationalExpr

三元表达式
TernaryExpr

```

