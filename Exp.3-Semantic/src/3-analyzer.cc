#include "3-analyzer.h"
#include "0-driver.h"
#include "patterns.hpp"

using namespace Limc;
using namespace mpark::patterns;

Semantic::Semantic(Driver &driver) : driver(driver) {}

void Semantic::try_insert(Token &name, Token &type) {
    auto &alias = tables.back().get_alias();
    auto  err   = tables.back().insert_symbol(Symbol(name, type, tables.size(), alias));
    if (err) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(name.get_loc().value())
            .report_msg("Symbol `" + name.get_value() + "` has already been declared. ");
    }
}

void Semantic::try_insert(Token &name, string &type) {
    auto err = tables.back().insert_symbol(
        Symbol(name.get_value(), type, tables.size(), tables.back().get_alias()));
    if (err) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(name.get_loc().value())
            .report_msg("Symbol `" + name.get_value() + "` has already been declared. ");
    }
}

void Semantic::try_insert(Token &name, Token &type, vector<Token> &paramaters) {
    vector<string> param_types;
    for (auto &param : paramaters) {
        // get parameters' type, [0] <- type name
        param_types.push_back(param.get_child(0).get_value());
    }
    auto err = tables.back().insert_symbol(
        Symbol(name, type, param_types, tables.size(), tables.back().get_alias()));
    if (err) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(name.get_loc().value())
            .report_msg("Symbol `" + name.get_value() + "` has already been declared. ");
    }
}

void Semantic::new_table(const string &name) { tables.emplace_back(name); }

void Semantic::new_table() { tables.emplace_back(); }

void Semantic::leave_table() {
    tables.pop_back();
    return;
}

string Semantic::check_ident(Token &root) {
    // 标识符的使用，包括变量与函数
    for (auto table = tables.rbegin(); table != tables.rend(); table++) {
        auto symbol = table->find_symbol(root.get_value());
        if (symbol != nullopt) {
            return symbol.value().type;
        }
    }
    driver.report()
        .report_level(Level::Error)
        .report_loc(root.get_loc().value())
        .report_msg("Variable `" + root.get_value() + "` is not defined in such scope.");
    return "";
}

string Semantic::check_binary_expr(Token &root) {
    // a + b | a & b | a * b
    // 0 1 2
    auto &l_value = root.get_child(0);
    auto  l_type  = check_ty(l_value);

    auto op = root.get_child(1).get_value();

    auto &r_value = root.get_child(2);
    auto  r_type  = check_ty(r_value);

    if (l_type != r_type) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(root.get_loc().value())
            .report_msg(
                "Type around operator `" + op + "` is not the same: `" + l_type + "` and `" +
                r_type + ".");
        return "ERROR_TYPE";
    }
    return l_type;
}

string Semantic::check_relational_expr(Token &root) {
    // a >= b | a == b | a != b
    // 0 1  2
    auto &l_value = root.get_child(0);
    auto  l_type  = check_ty(l_value);

    auto op = root.get_child(1).get_value();

    auto &r_value = root.get_child(2);
    auto  r_type  = check_ty(r_value);

    if (l_type != r_type) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(root.get_loc().value())
            .report_msg(
                "Type around operator `" + op + "` is not the same: `" + l_type + "` and `" +
                r_type + ".");
        return "ERROR_TYPE";
    }
    // int as bool
    // maybe fix later
    return "int";
}

string Semantic::check_assignment_expr(Token &root) {
    // a = 12 | a[10][9][8] = b[10][11]
    auto &l_value = root.get_child(0);
    auto  l_type  = check_ty(l_value);

    auto &r_value = root.get_child(2);
    auto  r_type  = check_ty(r_value);

    if (l_type != r_type) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(l_value.get_loc().value())
            .report_loc(r_value.get_loc().value())
            .report_msg(
                "Type around assignment operator `=` is not the same: `" + l_type + "` and `" +
                r_type + ".");
        return "ERROR_TYPE";
    }
    return l_type;
}
string Semantic::check_parenthesis_expr(Token &root) {
    // (expr)
    return check_ty(root.get_child(0));
}
string Semantic::check_prefix_expr(Token &root) {
    // ++a | *a | &a | !a
    // 0 1
    auto &node         = root.get_child(1);
    auto  op           = root.get_child(0).get_value();
    auto  vanilla_type = check_ty(node);
    // ty *a;
    // - +      =>  无约束
    // ! ~      =>  ty == int -> int
    // ++ --    =>  ty-ident -> int

    if (op == "--" || op == "++") {
        if (vanilla_type != "int") {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Operand type of prefix increment/decrement expression must be integer.");
            return "ERROR_TYPE";
        }
    } else if (op == "!" || op == "~") {
        if (vanilla_type != "int") {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Bitwise operand must perform on integer expression.");
            return "ERROR_TYPE";
        }
    }
    return vanilla_type;
}
string Semantic::check_postfix_expr(Token &root) {
    // a++ | a--
    auto &node         = root.get_child(0);
    auto  vanilla_type = check_ty(node);
    if (vanilla_type != "int") {
        driver.report()
            .report_level(Level::Error)
            .report_loc(root.get_loc().value())
            .report_msg("Operand type of postfix increment/decrement expression must be integer.");
        return "ERROR_TYPE";
    }
    return vanilla_type;
}
string Semantic::check_ternary_expr(Token &root) {
    // a ? b : c
    // 0 1 2 3 4
    // ^   ^   ^
    auto &cond_value = root.get_child(0);
    auto &yes_value  = root.get_child(2);
    auto &no_value   = root.get_child(4);

    auto cond_type = check_ty(cond_value);
    auto yes_type  = check_ty(yes_value);
    auto no_type   = check_ty(no_value);

    if (yes_type != no_type) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(root.get_loc().value())
            .report_msg(
                "Branches' type of ternary expression should be the same, got: `" + yes_type +
                "` and `" + no_type + "`.");
        return "ERROR_TYPE";
    }
    if (cond_type != "int") {
        driver.report()
            .report_level(Level::Error)
            .report_loc(root.get_loc().value())
            .report_msg(
                "Condition's type of ternary expression should be `int`, got: `" + cond_type +
                "`.");
        return "ERROR_TYPE";
    }
    return yes_type;
}
string Semantic::check_call_expr(Token &root) {
    // todo
    // fn(a, b, c)
    // 0  \--1--/

    /*
        1. 是否为函数
        2. 参数量匹配
        3. 参数类型匹配
    */
    auto &function = root.get_child(0);
    auto  args     = root.get_child(1).get_children();
    for (auto table = tables.rbegin(); table != tables.rend(); table++) {
        auto symbol = table->find_symbol(function.get_value());
        if (symbol != nullopt) {
            if (!symbol->is_func) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(function.get_loc().value())
                    .report_msg("Variable `" + function.get_value() + "` is not callable.");
                return "ERROR_TYPE";
            }
            auto params       = symbol.value().parameters;
            auto params_count = params.size();
            auto args_count   = args.size();
            if (params_count != args_count) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(function.get_loc().value())
                    .report_msg(
                        "Function `" + function.get_value() + "` requires " +
                        to_string(params_count) + " parameters, but " + to_string(args_count) +
                        " is given.");
                return "ERROR_TYPE";
            }
            for (int i = 0; i < args_count; i++) {
                auto type = check_ty(args[i]);
                if (type != params[i]) {
                    driver.report()
                        .report_level(Level::Error)
                        .report_loc(args[i].get_loc().value())
                        .report_msg(
                            "Function `" + function.get_value() + "` requires type `" + params[i] +
                            "` as parameter, but type `" + type + "` is given.");
                    return "ERROR_TYPE";
                }
            }
            return symbol.value().type;
        }
    }
    // 到这里说明没有在符号表中找到函数
    driver.report()
        .report_level(Level::Error)
        .report_loc(root.get_loc().value())
        .report_msg("Function `" + function.get_value() + "` is accessed before declaration.");
    return "ERROR_TYPE";
}
string Semantic::check_index_expr(Token &root) {
    // a[1][1][1]
    // 0 1  2  3 ...
    auto &tokens     = root.get_children();
    auto &array_name = tokens[0];
    auto  type       = check_ty(array_name);
    for (auto index = tokens.begin() + 1; index != tokens.end(); index++) {
        // 1. index -> int
        if (check_ty(index->get_child(0)) == "int") {
            // 2. is array(pointer of such type)
            if (type.back() != '*') {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(array_name.get_loc().value())
                    .report_msg("Type `" + type + "` can not be indexed.");
                return "ERROR_TYPE";
            }
            type.pop_back();
        } else {
            driver.report()
                .report_level(Level::Error)
                .report_loc(index->get_loc().value())
                .report_msg("Array index should be integer.");
            return "ERROR_TYPE";
        }
    }
    return type;
}

string Semantic::check_array_literal(Token &root) {
    string type;
    for (auto &item : root.get_children()) {
        auto current = check_ty(item);
        if (!type.empty() && current != type) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(item.get_loc().value())
                .report_msg("Item in array literal should be of the same type.");
            return "ERROR_TYPE";
        }
        type = current;
    }
    return type + "*";
}

string Semantic::check_ty(Token &root) {
    // TODO

    if (root.get_type() != "") {
        return root.get_type();
    }
    string res = "";
    match(root.get_kind())(
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

    root.set_type(res);
    return res;
}
/*
 pattern("")        = [&] { return (root); },
 */
void Semantic::walk_var_decl(Token &root) {
    // 声明部分
    /*
        int a, b, c = 12;
        float *a = &b;
    */
    auto  value    = root.get_value();
    auto &children = root.get_children();
    auto &type     = children[0];

    // [Type], [VarList]
    // VarList -> | VarDef | VarDecl
    for (auto &decl_def : children[1].get_children()) {
        auto &var = decl_def.get_child(0);

        auto l_type = type.get_value();
        auto name   = var.get_kind();
        if (name == "Identifier") {
            try_insert(var, type);
            check_ty(var);
        } else if (name == "IndexExpr") {
            // 定义数组
            l_type += string(var.get_children().size() - 1, '*');
            try_insert(var.get_child(0), l_type);
            check_ty(var);
        }

        if (decl_def == "VarDef") {
            auto &r_value = decl_def.get_child(2);
            auto  r_type  = check_ty(r_value);
            if (l_type != r_type) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(var.get_loc().value())
                    .report_loc(r_value.get_loc().value())
                    .report_msg("Cannot assign `" + r_type + "` to `" + l_type + "`.");
            }
        }
    }
}

void Semantic::walk_func_def(Token &root) {
    inner_return_ty.clear();
    auto &children   = root.get_children();
    auto &parameters = children[2].get_children();
    try_insert(children[1], children[0], parameters);
    new_table("F(" + children[1].get_value() + ")");
    for (auto &param : parameters) {
        try_insert(param.get_child(1), param.get_child(0));
        check_ty(param.get_child(1));
    }
    for (auto &child : children[3].get_children()) {
        walk(child);
    }
    auto ret_type = children[0].get_value();
    if (ret_type == inner_return_ty) {
    } else if (ret_type == "void" && inner_return_ty.empty()) {
    } else {
        // type not match, error
        driver.report()
            .report_level(Level::Error)
            .report_loc(children[0].get_loc().value())
            .report_msg(
                "Return type not match: Function `" + children[1].get_value() +
                "` requires return type: `" + ret_type + "`, but `" +
                (inner_return_ty.empty() ? "void" : inner_return_ty) + "` was given.");
    }
    leave_table();
}

void Semantic::walk_block(Token &root) {
    auto &children = root.get_children();
    new_table();
    for (auto &child : children) {
        walk(child);
    }
    leave_table();
}

void Semantic::walk(Token &root) {
    bool matched = true;
    match(root.get_kind())(
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
    auto &children = root.get_children();
    if (root == "ReturnStmt" && inner_return_ty.empty() && !children.empty()) {
        inner_return_ty = check_ty(children[0]);
    } else {
        optional<string> res = nullopt;

        res = check_ty(root);
        if (res != nullopt && res.value().empty()) {
            match(root.get_kind())(
                pattern("WhileStmt")   = [&] { ++loops; },
                pattern("DoWhileStmt") = [&] { ++loops; },
                pattern("ForStmt")     = [&] { ++loops; },
                pattern("SwitchStmt")  = [&] { ++switches; },
                pattern("ContinueStmt") =
                    [&] {
                        if (!loops) {
                            driver.report()
                                .report_level(Level::Error)
                                .report_loc(root.get_loc().value())
                                .report_msg("Continue should stay in loop blocks.");
                        }
                    },
                pattern("BreakStmt") =
                    [&] {
                        if (!loops && !switches) {
                            driver.report()
                                .report_level(Level::Error)
                                .report_loc(root.get_loc().value())
                                .report_msg("Break should stay in loop or switch blocks.");
                        }
                    },
                pattern(_) = [] {}

            );

            for (auto &child : children) {
                walk(child);
            }

            match(root.get_kind())(
                pattern("WhileStmt")   = [&] { --loops; },
                pattern("DoWhileStmt") = [&] { --loops; },
                pattern("ForStmt")     = [&] { --loops; },
                pattern("SwitchStmt")  = [&] { --switches; },
                pattern(_)             = [] {}

            );
        }
    }
}
