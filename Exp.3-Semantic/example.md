# Example

## Input

``` C
struct A {
    int a;
};

struct B {
    char     a;
    struct A b[5];
    int      c;
} t1;

int main() {
    struct B p[5];
    int      p1 = p[0].a;
    int      p2 = t1.b[2].a;
    return p1 + p2;
}
```

## Output

```LaTeX
Parse Finish!

==================================================
Scope: F(main)

<TABLE BEGIN>
      Name | Offset | Size | Align | Type
--------------------------------------------------
         p |      0 |  140 |     4 | [5;{a:char | b:[5;{a:int}] | c:int}]
        p1 |    140 |    4 |     4 | int
        p2 |    144 |    4 |     4 | int
<TABLE END>

==================================================
Scope: GLOBAL

<TABLE BEGIN>
      Name | Offset | Size | Align | Type
--------------------------------------------------
      main |      0 |    0 |     0 | Fn()->int
        t1 |      0 |   28 |     4 | {a:char | b:[5;{a:int}] | c:int}
<TABLE END>

{Program}
|---{Struct, A} (1,1)->(1,7)
|   `---{StructList}
|       `---{StructItemDecl}
|           |---{Type, int} (2,5)->(2,8)
|           `---{VarList}
|               `---{VarDecl}
|                   `---{Identifier, a} (2,9)->(2,10)
|---{GlobalVarDecl}
|   |---{Struct, B} (5,1)->(5,7)
|   |   `---{StructList}
|   |       |---{StructItemDecl}
|   |       |   |---{Type, char} (6,5)->(6,9)
|   |       |   `---{VarList}
|   |       |       `---{VarDecl}
|   |       |           `---{Identifier, a} (6,14)->(6,15)
|   |       |---{StructItemDecl}
|   |       |   |---{Struct, A} (7,5)->(7,11)
|   |       |   `---{VarList}
|   |       |       `---{VarDecl}
|   |       |           `---{IndexExpr}
|   |       |               |---{Identifier, b} (7,14)->(7,15)
|   |       |               `---{Index} (7,16)->(7,17)
|   |       |                   `---{IntegerLiteral, 5} (7,16)->(7,17)
|   |       `---{StructItemDecl}
|   |           |---{Type, int} (8,5)->(8,8)
|   |           `---{VarList}
|   |               `---{VarDecl}
|   |                   `---{Identifier, c} (8,14)->(8,15)
|   `---{VarList}
|       `---{VarDecl}
|           `---{Identifier, t1} [{a:char | b:[5;{a:int}] | c:int}] (9,3)->(9,5)
`---{FuncDef}
    |---{Type, int} (11,1)->(11,4)
    |---{Identifier, main} (11,5)->(11,9)
    |---{ParamList}
    `---{BlockStmt}
        |---{LocalVarDecl}
        |   |---{Struct, B} (12,5)->(12,11)
        |   `---{VarList}
        |       `---{VarDecl}
        |           `---{IndexExpr} [{a:char | b:[5;{a:int}] | c:int}]
        |               |---{Identifier, p} [[5;{a:char | b:[5;{a:int}] | c:int}]] (12,14)->(12,15)
        |               `---{Index} (12,16)->(12,17)
        |                   `---{IntegerLiteral, 5} [int] (12,16)->(12,17)
        |---{LocalVarDecl}
        |   |---{Type, int} (13,5)->(13,8)
        |   `---{VarList}
        |       `---{VarDef}
        |           |---{Identifier, p1} [int] (13,14)->(13,16)
        |           |---{Operator, =} (13,17)->(13,18)
        |           `---{MemberExpr} [char]
        |               |---{IndexExpr} [{a:char | b:[5;{a:int}] | c:int}]
        |               |   |---{Identifier, p} [[5;{a:char | b:[5;{a:int}] | c:int}]] (13,19)->(13,20)
        |               |   `---{Index} (13,21)->(13,22)
        |               |       `---{IntegerLiteral, 0} [int] (13,21)->(13,22)
        |               `---{Identifier, a} (13,24)->(13,25)
        |---{LocalVarDecl}
        |   |---{Type, int} (14,5)->(14,8)
        |   `---{VarList}
        |       `---{VarDef}
        |           |---{Identifier, p2} [int] (14,14)->(14,16)
        |           |---{Operator, =} (14,17)->(14,18)
        |           `---{MemberExpr} [int]
        |               |---{IndexExpr} [{a:int}]
        |               |   |---{MemberExpr} [[5;{a:int}]]
        |               |   |   |---{Identifier, t1} [{a:char | b:[5;{a:int}] | c:int}] (14,19)->(14,21)
        |               |   |   `---{Identifier, b} (14,22)->(14,23)
        |               |   `---{Index} (14,24)->(14,25)
        |               |       `---{IntegerLiteral, 2} [int] (14,24)->(14,25)
        |               `---{Identifier, a} (14,27)->(14,28)
        `---{ReturnStmt} (15,5)->(15,11)
            `---{ArithBinaryExpr} [int]
                |---{Identifier, p1} [int] (15,12)->(15,14)
                |---{Operator, +} (15,15)->(15,16)
                `---{Identifier, p2} [int] (15,17)->(15,19)
```
