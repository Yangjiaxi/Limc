#ifndef VARIABLE_TABLE_H
#define VARIABLE_TABLE_H

#include "0-token.h"
#include "3-type.h"
#include <unordered_map>
#include <utility>

namespace Limc {

// Var(Lvar | GVar) | Func | StringLiteral
class Symbol {
  public:
    // Symbol(string name, string type, int scope, string scope_alias);
    //
    // Symbol(Token &name, Token &type, int scope, string scope_alias);
    //
    // Symbol(Token &name, Token &type, vector<string> parameters, int scope, string scope_alias);

    Symbol(string name, Type type, int scope, string scope_alias);

    string name;
    string scope_alias;
    int    scope;
    // 类型，对于所有都有效
    // 平凡类型 | 数组 | 结构体 | 函数
    Type type;

    // 对于整个函数，有一个栈大小
    int stack_size; // for func

    // 对于每个变量，有一个基址偏移
    int offset; // for Lvar and Gvar
};

class SymbolTable {
  public:
    explicit SymbolTable(string name) : name(std::move(name)) {}

    SymbolTable() = default;

    string &get_alias() { return name; };

    int insert_symbol(const Symbol &symbol);

    optional<Symbol> find_symbol(const string &name);

  private:
    string                        name;
    unordered_map<string, Symbol> table;
};
} // namespace Limc

#endif