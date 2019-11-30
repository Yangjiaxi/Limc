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

    Symbol(const Token &name, const Token &type, int scope, string scope_alias);

    Symbol(
        const Token &  name,
        const Token &  type,
        vector<string> parameters,
        int            scope,
        string         scope_alias);

    string         name;
    string         type;
    vector<string> parameters;
    string         scope_alias; // for debug
    int            scope;
    bool           isFunction;
};

class SymbolTable {
  public:
    int insert_symbol(const Symbol &symbol);

    optional<Symbol> find_symbol(const string &name);

  private:
    unordered_map<string, Symbol> table;
};
} // namespace Limc

#endif