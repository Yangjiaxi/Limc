#include "3-analyzer.h"
#include "0-driver.h"

using namespace Limc;

Semantic::Semantic(Driver &driver) : driver(driver) {}

runtime_error Semantic::semantic_error(const string &msg, const Token &token) {
    return runtime_error(Driver::gen_error(msg, token.get_loc().value()));
}

void Semantic::try_insert(const Token &name, const Token &type) {
    stringstream s;
    s << "Scope - {" << name.get_loc().value() << "}";
    auto err = tables.back().insert_symbol(Symbol(name, type, tables.size(), s.str()));
    if (err) {
        driver.add_error("Symbol " + name.get_value() + " has already been declared. ", name);
    }
}

void Semantic::try_insert(const Token &name, const string &type) {
    stringstream s;
    s << "Scope - {" << name.get_loc().value() << "}";
    auto err = tables.back().insert_symbol(Symbol(name.get_value(), type, tables.size(), s.str()));
    if (err) {
        driver.add_error("Symbol " + name.get_value() + " has already been declared. ", name);
    }
}

void Semantic::try_insert(const Token &name, const Token &type, const vector<Token> &paramaters) {
    stringstream   s;
    vector<string> param_types;
    for (const auto &param : paramaters) {
        // get parameters' type, [0] <- type name
        param_types.push_back(param.get_child(0).get_value());
    }
    s << "Scope - {" << name.get_loc().value() << "}";
    auto err = tables.back().insert_symbol(Symbol(name, type, param_types, tables.size(), s.str()));
    if (err) {
        driver.add_error("Symbol " + name.get_value() + " has already been declared. ", name);
    }
}
void Semantic::walk(const Token &root) { driver.add_error("not implemented"); }
