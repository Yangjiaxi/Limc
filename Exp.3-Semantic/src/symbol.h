#ifndef VARIABLE_TABLE_H
#define VARIABLE_TABLE_H

#include "token.h"
#include <unordered_map>

namespace Limc {

class Symbol {
  public:
    Symbol(string name, string type, int scope);

    Symbol(const Token &name, const Token &type, int scope);

    Symbol(
        const Token &name, const Token &type, vector<string> parameters,
        int scope);

    string name;
    string type;
    vector<string> parameters;
    int scope;
    bool isFunction;
};

class SymbolTable {
  public:
    int insertSymbol(const Symbol &symbol);

    optional<Symbol> findSymbol(const string &name);

  private:
    unordered_map<string, Symbol> table;
};
} // namespace Limc

#endif