#include "3-symbol.h"
#include "optional"

using namespace Limc;

Symbol::Symbol(string name, string type, int scope, string scope_alias)
    : name(move(name)), type(move(type)), scope(scope), parameters(),
      scope_alias(move(scope_alias)) {}

Symbol::Symbol(const Token &name, const Token &type, int scope, string scope_alias)
    : name(name.get_value()), type(type.get_value()), scope(scope), parameters(),
      scope_alias(move(scope_alias)) {}

Symbol::Symbol(
    const Token &name, const Token &type, vector<string> parameters, int scope, string scope_alias)
    : name(name.get_value()), type(type.get_name()), parameters(move(parameters)), scope(scope),
      scope_alias(move(scope_alias)) {}

int SymbolTable::insert_symbol(const Symbol &symbol) {
    if (table.find(symbol.name) != table.end()) {
        // Already exist in this scope
        return 1;
    }
    // otherwise, insert
    table.insert({symbol.name, symbol});
    return 0;
}

// get Symbol w.r.t its name
// return Symbol if exist
// otherwise return null(opt)
optional<Symbol> SymbolTable::find_symbol(const string &name) {
    try {
        return table.at(name);
    } catch (...) {
        return nullopt;
    }
}
