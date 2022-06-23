#ifndef VARIABLE_TABLE_H
#define VARIABLE_TABLE_H

#include "0-token.h"
#include "3-type.h"
#include <map>
#include <utility>

namespace Limc {

// Var(Lvar | GVar) | Func | StringLiteral
class Symbol {
  public:
    Symbol(string name, Type *type, unsigned offset);

    // 变量的名字
    string name;

    // 类型，对于所有都有效
    // 平凡类型 | 数组 | 结构体 | 函数
    Type *type;

    // 对于整个函数，有一个栈大小
    int stack_size; // for Type.c_type == Ctype::Func

    // 对于每个变量，有一个基址偏移
    unsigned offset; // for others
};

// 一个作用域的符号表
class SymbolTable {
  public:
    explicit SymbolTable(string name) : name(move(name)) {}

    SymbolTable() = default;

    string &get_alias() { return name; };

    int insert_symbol(const Symbol &symbol);

    optional<Symbol> find_symbol(const string &name);

    string describe() const;

    static string table_head();

  private:
    string              name;
    map<string, Symbol> table;
};
} // namespace Limc

#endif