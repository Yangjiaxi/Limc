#include "3-analyzer.h"
#include "0-driver.h"
#include "patterns.hpp"

using namespace Limc;
using namespace mpark::patterns;

Semantic::Semantic(Driver &driver) : driver(driver) {}

runtime_error Semantic::semantic_error(const string &msg, const Token &token) {
    return runtime_error(Driver::gen_error(msg, token.get_loc().value()));
}

void Semantic::try_insert(const Token &name, const Token &type) {
    stringstream s;
    s << "{" << name.get_loc().value() << "}";
    auto err = tables.back().insert_symbol(Symbol(name, type, tables.size(), s.str()));
    if (err) {
        driver.add_error("Symbol `" + name.get_value() + "` has already been declared. ", name);
    }
}

void Semantic::try_insert(const Token &name, const string &type) {
    stringstream s;
    s << "{" << name.get_loc().value() << "}";
    auto err = tables.back().insert_symbol(Symbol(name.get_value(), type, tables.size(), s.str()));
    if (err) {
        driver.add_error("Symbol `" + name.get_value() + "` has already been declared. ", name);
    }
}

void Semantic::try_insert(const Token &name, const Token &type, const vector<Token> &paramaters) {
    stringstream   s;
    vector<string> param_types;
    for (const auto &param : paramaters) {
        // get parameters' type, [0] <- type name
        param_types.push_back(param.get_child(0).get_value());
    }
    s << "{" << name.get_loc().value() << "}";
    auto err = tables.back().insert_symbol(Symbol(name, type, param_types, tables.size(), s.str()));
    if (err) {
        driver.add_error("Symbol `" + name.get_value() + "` has already been declared. ", name);
    }
}

void Semantic::new_table() {
    tables.emplace_back();
    return;
}

void Semantic::leave_table() {
    tables.pop_back();
    return;
}

string Semantic::check_ty(const Token &token) {
    // TODO
    return token.get_value();
}

// string Semantic::gen_type(const Token &type_token) {}

void Semantic::walk_var_decl(const Token &root) {
    // 声明部分
    /*
        int a, b, c = 12;
        float *a = &b;
    */
    auto value    = root.get_value();
    auto children = root.get_children();
    auto type     = children[0];

    for (const auto &decl_def : children[1].get_children()) {
        auto var = decl_def.get_child(0);
        try {
            auto l_type = type.get_value();
            match(var.get_name())(
                pattern("Identifier") = [&] { try_insert(var, type); },
                pattern("IndexExpr") =
                    [&] {
                        l_type += string(var.get_children().size() - 1, '*');
                        try_insert(var.get_child(0), l_type);
                        check_ty(var);
                    },
                pattern("VarDef") =
                    [&] {
                        auto r_value = decl_def.get_child(2);
                        auto r_type  = check_ty(r_value);
                        if (l_type != r_type) {
                            throw semantic_error(
                                "Cannot assign " + r_type + " to " + l_type + ".", r_value);
                        }
                    }

            );
        } catch (runtime_error &e) {
            driver.add_error(e.what());
        }
    }
}

void Semantic::walk_func_def(const Token &root) {
    inner_return_ty.clear();
    auto children   = root.get_children();
    auto parameters = children[2].get_children();
    try_insert(children[1], children[0], parameters);
    new_table();
    for (const auto &param : parameters) {
        try_insert(param.get_child(1), param.get_child(0));
    }
    for (const auto &child : children[3].get_children()) {
        walk(child);
    }
    auto ret_type = children[0].get_value();
    if (ret_type == inner_return_ty) {
    } else if (ret_type == "void" && inner_return_ty.empty()) {
    } else {
        // type not match, error
        driver.add_error(
            "Return type not match: Function `" + children[1].get_value() +
            "` requires return type: `" + ret_type + "`, but `" +
            (inner_return_ty.empty() ? "void" : inner_return_ty) + "` was given.");
    }
    leave_table();
}

void Semantic::walk_block(const Token &root) {
    auto children = root.get_children();
    new_table();
    for (const auto &child : children) {
        walk(child);
    }
    leave_table();
}

void Semantic::walk_return_stmt(const Token &root) {
    auto children = root.get_children();
    if (inner_return_ty.empty() && !children.empty()) {
        // return check
    } else {
        walk_multi_stmt(root);
    }
}

void Semantic::walk_multi_stmt(const Token &root) { cout << "In multi Stmt" << endl; }

void Semantic::walk(const Token &root) {
    match(root.get_name())(
        pattern("GlobalVarDecl") = [&] { walk_var_decl(root); },
        pattern("LocalVarDecl")  = [&] { walk_var_decl(root); },
        pattern("FuncDef")       = [&] { walk_func_def(root); },
        pattern("BlockStmt")     = [&] { walk_block(root); },
        pattern("Program")       = [&] { walk_block(root); },
        pattern("ReturnStmt")    = [&] { walk_return_stmt(root); }

    );
}
