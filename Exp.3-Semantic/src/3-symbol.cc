#include "3-symbol.h"

#include "0-color.h"
#include "optional"
#include "util.h"

using namespace Limc;
using namespace std;

Symbol::Symbol(string name, Type type, int scope, string scope_alias)
    : name(move(name)), type(move(type)), scope(scope), scope_alias(move(scope_alias)) {}

// int SymbolTable::insert_symbol(const Symbol &symbol) {
//     // otherwise, insert
//     stringstream ss;
//
//     build_str(ss, " | ", RED);
//     build_str(ss, symbol.name, BLUE, 7);
//
//     build_str(ss, " | ", RED);
//     build_str(ss, std::to_string(symbol.scope), YELLOW, 5);
//
//     build_str(ss, " | ", RED);
//     string alias = symbol.scope_alias;
//     if (alias.empty()) {
//         if (symbol.scope > 1)
//             alias = "BLOCK";
//         else
//             alias = "GLOBAL";
//     }
//     build_str(ss, alias, BOLD_YELLOW, 10);
//
//     build_str(ss, " | ", RED);
//     build_str(ss, symbol.type, GREEN, 15);
//
//     build_str(ss, " | ", RED);
//     build_str(ss, symbol.is_func ? "function" : "", MAGENTA, 8);
//
//     build_str(ss, " | ", RED);
//
//     for (const auto &param : symbol.parameters) {
//         build_str(ss, param, CYAN);
//         if (&param != &symbol.parameters.back())
//             build_str(ss, ",", YELLOW);
//     }
//
//     cout << ss.str() << endl;
//
//     if (table.find(symbol.name) != table.end()) {
//         // Already exist in this scope
//         return 1;
//     }
//     table.insert({symbol.name, symbol});
//     return 0;
// }

// get Symbol w.r.t its name
// return Symbol if exist
// otherwise return null(opt)
optional<Symbol> SymbolTable::find_symbol(const string &name) {
    try {
        return table.at(name);
    } catch (...) { return nullopt; }
}
