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
    string line(75, '-');
    cout << line << endl;
}

void Semantic::new_table() {
    tables.emplace_back();
    string line(75, '-');
    cout << line << endl;
}

void Semantic::leave_table() {
    tables.pop_back();
    return;
}

string Semantic::check_ident(const Token &root) {
    // 标识符的使用，包括变量与函数
    for (auto table = tables.rbegin(); table != tables.rend(); table++) {
        auto symbol = table->find_symbol(root.get_value());
        if (symbol != nullopt) {
            return symbol.value().type;
        }
    }
    throw semantic_error("Variable `" + root.get_value() + "` is not defined in such scope.", root);
}
string Semantic::check_binary_expr(const Token &root) {
    // a + b | a & b | a * b
    // 0 1 2
    auto l_value = root.get_child(0);
    auto l_type  = check_ty(l_value);

    auto op = root.get_child(1).get_value();

    auto r_value = root.get_child(2);
    auto r_type  = check_ty(r_value);

    if (l_type != r_type) {
        throw semantic_error(
            "Type around operator `" + op + "` is not the same: `" + l_type + "` and `" + r_type +
                ".",
            root);
    }
    return l_type;
}

string Semantic::check_relational_expr(const Token &root) {
    // a >= b | a == b | a != b
    // 0 1  2
    auto l_value = root.get_child(0);
    auto l_type  = check_ty(l_value);

    auto op = root.get_child(1).get_value();

    auto r_value = root.get_child(2);
    auto r_type  = check_ty(r_value);

    if (l_type != r_type) {
        throw semantic_error(
            "Type around operator `" + op + "` is not the same: `" + l_type + "` and `" + r_type +
                ".",
            root);
    }
    // int as bool
    // maybe fix later
    return "int";
}

string Semantic::check_assignment_expr(const Token &root) {
    // a = 12 | a[10][9][8] = b[10][11]
    auto l_value = root.get_child(0);
    auto l_type  = check_ty(l_value);

    auto r_value = root.get_child(2);
    auto r_type  = check_ty(r_value);

    if (l_type != r_type) {
        throw semantic_error(
            "Type around assignment operator `=` is not the same: `" + l_type + "` and `" + r_type +
                ".",
            root);
    }
    return l_type;
}
string Semantic::check_parenthesis_expr(const Token &root) {
    // (expr)
    return check_ty(root.get_child(0));
}
string Semantic::check_prefix_expr(const Token &root) {
    // ++a | *a | &a | !a
    // 0 1
    auto node         = root.get_child(1);
    auto op           = root.get_child(0).get_value();
    auto vanilla_type = check_ty(node);
    // ty *a;
    // *a       =>  -> ty
    // &a       =>  -> ty**
    // - +      =>  无约束
    // ! ~      =>  ty == int -> int
    // ++ --    =>  ty-ident -> int

    /*  e.g
        int ***a;
        ***a = 12;
        ^^^
        123
        * (**a) => * (* (*a)) => * (* (* (int***))) => * (* (int**)) => * (int*) => int
    */
    if (op == "*") {
        if (vanilla_type.back() == '*') {
            return vanilla_type.substr(0, vanilla_type.size() - 1);
        } else {
            throw semantic_error(
                "Can't defer at non-pointer variable: `" + node.get_value() + "`.", root);
        }
    } else if (op == "&") {
        return vanilla_type + "*";
    } else if (op == "--" || op == "++") {
        if (vanilla_type != "int") {
            throw semantic_error(
                "Operand type of prefix increment/decrement expression must be "
                "integer.",
                root);
        }
    } else if (op == "!" || op == "~") {
        if (vanilla_type != "int") {
            throw semantic_error("Bitwise operand must perform on integer expression.", root);
        }
    }
    return vanilla_type;
}
string Semantic::check_postfix_expr(const Token &root) {
    // a++ | a--
    auto node         = root.get_child(0);
    auto vanilla_type = check_ty(node);
    if (vanilla_type != "int") {
        throw semantic_error(
            "Operand type of postfix increment/decrement expression must be "
            "integer.",
            root);
    }
    return vanilla_type;
}
string Semantic::check_ternary_expr(const Token &root) {
    // a ? b : c
    // 0 1 2 3 4
    // ^   ^   ^
    auto cond_value = root.get_child(0);
    auto yes_value  = root.get_child(2);
    auto no_value   = root.get_child(4);

    auto cond_type = check_ty(cond_value);
    auto yes_type  = check_ty(yes_value);
    auto no_type   = check_ty(no_value);

    if (yes_type != no_type) {
        throw semantic_error(
            "Branches' type of ternary expression should be the same, got: `" + yes_type +
                "` and `" + no_type + "`.",
            yes_value);
    }
    if (cond_type != "int") {
        throw semantic_error(
            "Condition's type of ternary expression should be `int`, got: `" + cond_type + "`.",
            cond_value);
    }
    return yes_type;
}
string Semantic::check_call_expr(const Token &root) {
    // todo
    // fn(a, b, c)
    // 0  \--1--/

    /*
        1. 是否为函数
        2. 参数量匹配
        3. 参数类型匹配
    */
    auto function = root.get_child(0);
    auto args     = root.get_child(1).get_children();
    for (auto table = tables.rbegin(); table != tables.rend(); table++) {
        auto symbol = table->find_symbol(function.get_value());
        if (symbol != nullopt) {
            if (!symbol->is_func) {
                throw semantic_error(
                    "Variable `" + function.get_value() + "` is not callable.", function);
            }
            auto params       = symbol.value().parameters;
            auto params_count = params.size();
            auto args_count   = args.size();
            if (params_count != args_count) {
                throw semantic_error(
                    "Function `" + function.get_value() + "` requires " + to_string(params_count) +
                        " parameters, but " + to_string(args_count) + " is given.",
                    function);
            }
            for (int i = 0; i < args_count; i++) {
                auto type = check_ty(args[i]);
                if (type != params[i]) {
                    throw semantic_error(
                        "Function `" + function.get_value() + "` requires type `" + params[i] +
                            "` as parameter, but type `" + type + "` is given.",
                        args[i]);
                }
            }
            return symbol.value().type;
        }
    }
    throw semantic_error(
        "Function `" + function.get_value() + "` is accessed before declaration.", root);
}
string Semantic::check_index_expr(const Token &root) {
    // a[1][1][1]
    // 0 1  2  3 ...
    auto tokens     = root.get_children();
    auto array_name = tokens[0];
    auto type       = check_ty(array_name);
    for (auto index = tokens.begin() + 1; index != tokens.end(); index++) {
        // 1. index -> int
        if (check_ty(index->get_child(0)) == "int") {
            // 2. is array(pointer of such type)
            if (type.back() != '*') {
                throw semantic_error(type + " can not be indexed.", *index);
            }
            type.pop_back();
        } else {
            throw semantic_error("Array index should be integer.", *index);
        }
    }
    return type;
}

string Semantic::check_array_literal(const Token &root) {
    string type;
    for (const auto &item : root.get_children()) {
        auto current = check_ty(item);
        if (!type.empty() && current != type) {
            throw semantic_error("Item in array literal should be of the same type.", item);
        }
        type = current;
    }
    return type + "*";
}

string Semantic::check_ty(const Token &root) {
    // TODO
    string res = "";
    match(root.get_name())(
        pattern("Identifier")             = [&] { res = check_ident(root); },
        pattern("BinaryExpr")             = [&] { res = check_binary_expr(root); },
        pattern("RelationalExpr")         = [&] { res = check_relational_expr(root); },
        pattern("AssignmentExpr")         = [&] { res = check_assignment_expr(root); },
        pattern("CompoundAssignmentExpr") = [&] { res = check_assignment_expr(root); },
        pattern("ParenthesisExpr")        = [&] { res = check_parenthesis_expr(root); },
        pattern("PrefixExpr")             = [&] { res = check_prefix_expr(root); },
        pattern("PostfixExpr")            = [&] { res = check_postfix_expr(root); },
        pattern("TernaryExpr")            = [&] { res = check_ternary_expr(root); },
        pattern("CallExpr")               = [&] { res = check_call_expr(root); },
        pattern("IndexExpr")              = [&] { res = check_index_expr(root); },
        pattern("ArrayLiteral")           = [&] { res = check_array_literal(root); },
        pattern("IntegerLiteral")         = [&] { res = "int"; },
        pattern("FloatLiteral")           = [&] { res = "float"; },
        pattern("CharLiteral")            = [&] { res = "char"; },
        pattern("StringLiteral")          = [&] { res = "char*"; },
        pattern(_)                        = [&] { res = ""; });

    return res;
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

    // [Type], [VarList]
    // VarList -> | VarDef | VarDecl
    for (const auto &decl_def : children[1].get_children()) {
        auto var = decl_def.get_child(0);
        try {
            auto l_type = type.get_value();
            auto name   = var.get_name();
            if (name == "Identifier") {
                try_insert(var, type);
            } else if (name == "IndexExpr") {
                l_type += string(var.get_children().size() - 1, '*');
                try_insert(var.get_child(0), l_type);
                check_ty(var);
            }

            if (decl_def == "VarDef") {
                auto r_value = decl_def.get_child(2);
                auto r_type  = check_ty(r_value);
                if (l_type != r_type) {
                    throw semantic_error(
                        "Cannot assign `" + r_type + "` to `" + l_type + "`.", r_value);
                }
            }

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
        pattern("FuncDecl")      = [&] {},
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
