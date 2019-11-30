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
    auto &alias = tables.back().get_alias();
    auto  err   = tables.back().insert_symbol(Symbol(name, type, tables.size(), alias));
    if (err) {
        driver.add_error("Symbol `" + name.get_value() + "` has already been declared. ", name);
    }
}

void Semantic::try_insert(const Token &name, const string &type) {
    auto err = tables.back().insert_symbol(
        Symbol(name.get_value(), type, tables.size(), tables.back().get_alias()));
    if (err) {
        driver.add_error("Symbol `" + name.get_value() + "` has already been declared. ", name);
    }
}

void Semantic::try_insert(const Token &name, const Token &type, const vector<Token> &paramaters) {
    vector<string> param_types;
    for (const auto &param : paramaters) {
        // get parameters' type, [0] <- type name
        param_types.push_back(param.get_child(0).get_value());
    }
    auto err = tables.back().insert_symbol(
        Symbol(name, type, param_types, tables.size(), tables.back().get_alias()));
    if (err) {
        driver.add_error("Symbol `" + name.get_value() + "` has already been declared. ", name);
    }
}

void Semantic::new_table(const string &name) {
    tables.emplace_back(name);
    return;
}

void Semantic::new_table() {
    tables.emplace_back();
    return;
}

void Semantic::leave_table() {
    tables.pop_back();
    return;
}

string Semantic::check_ident(const Token &root) {
    // todo
}
string Semantic::check_binary_expr(const Token &root) {
    // todo
}
string Semantic::check_assignment_expr(const Token &root) {
    // todo
}
string Semantic::check_parenthesis_expr(const Token &root) {
    // todo
}
string Semantic::check_prefix_expr(const Token &root) {
    // todo
}
string Semantic::check_postfix_expr(const Token &root) {
    // todo
}
string Semantic::check_ternary_expr(const Token &root) {
    // todo
}
string Semantic::check_call_expr(const Token &root) {
    // todo
}
string Semantic::check_index_expr(const Token &root) {
    // todo
}

string Semantic::check_array_literal(const Token &root) {
    // todo
}

string Semantic::check_ty(const Token &root) {
    // TODO
    return match(root.get_value())(
        pattern("Identifier")             = [&] { return check_ident(root); },
        pattern("BinaryExpr")             = [&] { return check_binary_expr(root); },
        pattern("RelationalExpr")         = [&] { return check_binary_expr(root); },
        pattern("AssignmentExpr")         = [&] { return check_assignment_expr(root); },
        pattern("CompoundAssignmentExpr") = [&] { return check_assignment_expr(root); },
        pattern("ParenthesisExpr")        = [&] { return check_parenthesis_expr(root); },
        pattern("PrefixExpr")             = [&] { return check_prefix_expr(root); },
        pattern("PostfixExpr")            = [&] { return check_postfix_expr(root); },
        pattern("TernaryExpr")            = [&] { return check_ternary_expr(root); },
        pattern("CallExpr")               = [&] { return check_call_expr(root); },
        pattern("IndexExpr")              = [&] { return check_index_expr(root); },
        pattern("ArrayLiteral")           = [&] { return check_array_literal(root); },
        pattern("IntegerLiteral")         = [] { return "int"; },
        pattern("FloatLiteral")           = [] { return "float"; },
        pattern("CharLiteral")            = [] { return "char"; },
        pattern(_)                        = [] { return ""; }

    );
}
/*
 pattern("")        = [&] { return (root); },
 */
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
    new_table("F(" + children[1].get_value() + ")");
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

void Semantic::walk(const Token &root) {
    bool matched = true;
    match(root.get_name())(
        pattern("GlobalVarDecl") = [&] { walk_var_decl(root); },
        pattern("LocalVarDecl")  = [&] { walk_var_decl(root); },
        pattern("FuncDef")       = [&] { walk_func_def(root); },
        pattern("BlockStmt")     = [&] { walk_block(root); },
        pattern("Program")       = [&] { walk_block(root); },
        pattern(_)               = [&] { matched = false; }

    );
    if (matched)
        return;
    auto children = root.get_children();
    if (root == "ReturnStmt" && inner_return_ty.empty() && !children.empty()) {
        try {
            inner_return_ty = check_ty(children[0]);
        } catch (runtime_error &e) {
            driver.add_error(e.what());
        }
    } else {
        optional<string> res = nullopt;
        try {
            res = check_ty(root);
        } catch (runtime_error &e) {
            driver.add_error(e.what());
        }
        if (res != nullopt && res.value().empty()) {
            cout << "Type: " << root.get_name() << endl;
            match(root.get_name())(
                pattern("WhileStmt")   = [&] { ++loops; },
                pattern("DoWhileStmt") = [&] { ++loops; },
                pattern("ForStmt")     = [&] { ++loops; },
                pattern("SwitchStmt")  = [&] { ++switchs; },
                pattern("ContinueStmt") =
                    [&] {
                        if (!loops) {
                            driver.add_error("Continue should stay in loop blocks.", root);
                        }
                    },
                pattern("BreakStmt") =
                    [&] {
                        if (!loops && !switchs) {
                            driver.add_error("Break should stay in loop or switch blocks.", root);
                        }
                    },
                pattern(_) = [] {}

            );

            for (const auto &child : children) {
                walk(child);
            }

            match(root.get_name())(
                pattern("WhileStmt")   = [&] { --loops; },
                pattern("DoWhileStmt") = [&] { --loops; },
                pattern("ForStmt")     = [&] { --loops; },
                pattern("SwitchStmt")  = [&] { --switchs; },
                pattern(_)             = [] {}

            );
        }
    }
}
