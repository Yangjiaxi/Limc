#include "3-symbol.h"
#include "optional"
#include "util.h"

// #define DEBUG

using namespace Limc;
using namespace std;

Symbol::Symbol(string name, Type *type, unsigned offset)
    : name(move(name)), type(type), offset(offset) {}

int SymbolTable::insert_symbol(const Symbol &symbol) {
    if (table.find(symbol.name) != table.end()) {
        return 1;
    }
#ifdef DEBUG
    cout << "vvvvvvvvvvvv Insert Symbol vvvvvvvvvvvv" << endl;
    cout << "\tScope\t: " << name << endl;
    cout << "\tName\t: " << symbol.name << endl;
    cout << "\tType\t: " << symbol.type->to_string() << endl;
    cout << "\tOffset\t: " << symbol.offset << endl;
    cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
#endif
    table.insert({symbol.name, symbol});
    return 0;
}

// get Symbol w.r.t its name
// return Symbol if exist
// otherwise return null(opt)
optional<Symbol> SymbolTable::find_symbol(const string &name) {
    try {
        return table.at(name);
    } catch (...) { return nullopt; }
}

string SymbolTable::describe() const {
    stringstream ss;
    for (auto &[name, symbol] : table) {
        build_str(ss, name, BOLD_BLUE, 10);
        ss << " | ";
        build_str(ss, to_string(symbol.offset), BOLD_CYAN, 6);
        ss << " | ";
        build_str(ss, to_string(symbol.type->size), BOLD_CYAN, 4);
        ss << " | ";
        build_str(ss, to_string(symbol.type->align), BOLD_CYAN, 5);
        ss << " | ";
        build_str(ss, symbol.type->to_string());
        ss << endl;
    }
    return ss.str();
}

string SymbolTable::table_head() {
    stringstream ss;
    build_str(ss, "Name", BOLD_BLUE, 10);
    ss << " | ";
    build_str(ss, "Offset", BOLD_CYAN, 6);
    ss << " | ";
    build_str(ss, "Size", BOLD_CYAN, 4);
    ss << " | ";
    build_str(ss, "Align", BOLD_CYAN, 5);
    ss << " | ";
    build_str(ss, "Type", BOLD_GREEN);
    ss << endl << string(50, '-') << endl;
    return ss.str();
}