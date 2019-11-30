#include "3-symbol.h"
#include "0-color.h"
#include "optional"
#include <iomanip>

using namespace Limc;

static stringstream &
build_str(stringstream &ss, const string &value, const char *color, const unsigned width = 0) {
    if (width)
        ss << color << setw(width) << value << RESET_COLOR;
    else
        ss << color << value << RESET_COLOR;
    return ss;
}

static stringstream &build_str(stringstream &ss, const string &value, const unsigned width = 0) {
    if (width)
        ss << setw(width) << value << RESET_COLOR;
    else
        ss << value << RESET_COLOR;
    return ss;
}

Symbol::Symbol(string name, string type, int scope, string scope_alias)
    : name(move(name)), type(move(type)), scope(scope), parameters(), is_func(false),
      scope_alias(move(scope_alias)) {}

Symbol::Symbol(const Token &name, const Token &type, int scope, string scope_alias)
    : name(name.get_value()), type(type.get_value()), scope(scope), parameters(), is_func(false),
      scope_alias(move(scope_alias)) {}

Symbol::Symbol(
    const Token &name, const Token &type, vector<string> parameters, int scope, string scope_alias)
    : name(name.get_value()), type(type.get_value()), parameters(move(parameters)), scope(scope),
      is_func(true), scope_alias(move(scope_alias)) {}

int SymbolTable::insert_symbol(const Symbol &symbol) {
    // otherwise, insert
    stringstream ss;

    build_str(ss, " | ", RED);
    build_str(ss, symbol.name, BLUE, 7);

    build_str(ss, " | ", RED);
    build_str(ss, std::to_string(symbol.scope), YELLOW, 3);

    build_str(ss, " | ", RED);
    string alias = symbol.scope_alias;
    if (alias.empty()) {
        if (symbol.scope > 1)
            alias = "BLOCK";
        else
            alias = "GLOBAL";
    }
    build_str(ss, alias, BOLD_YELLOW, 10);

    build_str(ss, " | ", RED);
    build_str(ss, symbol.type, GREEN, 15);

    build_str(ss, " | ", RED);
    build_str(ss, symbol.is_func ? "function" : "", MAGENTA, 8);

    build_str(ss, " | ", RED);

    for (const auto &param : symbol.parameters) {
        build_str(ss, param, CYAN);
        if (&param != &symbol.parameters.back())
            build_str(ss, ",", YELLOW);
    }

    cout << ss.str() << endl;

    if (table.find(symbol.name) != table.end()) {
        // Already exist in this scope
        return 1;
    }
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
