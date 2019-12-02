#ifndef VARIABLE_TABLE_H
#define VARIABLE_TABLE_H

#include "0-token.h"
#include <unordered_map>

namespace Limc {

// class Driver;

// Var(Lvar | GVar) | Func | StringLiteral
class Symbol {
  public:
    Symbol(string name, string type, int scope, string scope_alias);

    Symbol(Token &name, Token &type, int scope, string scope_alias);

    Symbol(Token &name, Token &type, vector<string> parameters, int scope, string scope_alias);

    string         name;
    string         type;
    vector<string> parameters;
    string         scope_alias; // for debug
    int            scope;
    bool           is_func;
};

class SymbolTable {
  public:
    SymbolTable(const string &name) : name(name) {}

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