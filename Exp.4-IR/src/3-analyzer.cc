#include "3-analyzer.h"
#include "0-driver.h"
#include "util.h"

// #define DEBUG

using namespace Limc;

Semantic::Semantic(Driver &driver)
    : driver(driver), inner_return_ty(nullptr), current_table(nullptr) {}

void Semantic::enter_scope(const string &name) {
    tables.emplace_back(name);
    current_table = &tables.back();
#ifdef DEBUG
    cout << ">>> Enter Scope\t: [" << name << "]" << endl;
#endif
}

void Semantic::leave_scope(unsigned frame_size) {
#ifdef DEBUG
    cout << "<<< Leave Scope\t: [" << tables.back().get_alias() << "]"
         << " Local Stack Size: " << local_stack_size << endl;
#endif
    string line(50, '=');

    cout << endl << line << endl;
    cout << "Scope: " << BOLD_YELLOW << current_table->get_alias() << RESET_COLOR << endl;

    cout << "Frame Size: " << BOLD_WHITE << frame_size << RESET_COLOR << endl;

    cout << endl << BOLD_RED << "<TABLE BEGIN>" << RESET_COLOR << endl;

    cout << SymbolTable::table_head();
    cout << current_table->describe();

    cout << BOLD_RED << "<TABLE END>" << RESET_COLOR << endl;

    tables.pop_back();
    current_table = &tables.back();
}

void Semantic::try_insert_symbol(Token &identifier, Type *type, bool is_glb, unsigned offset) {
    auto already_exist =
        current_table->insert_symbol(Symbol(identifier.get_value(), type, offset, is_glb));
    if (already_exist) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(identifier.get_loc().value())
            .report_msg("Symbol `" + identifier.get_value() + "` has already been declared. ", 2);
    }
}

Type *Semantic::expr(Token &root) {
    // 返回表达式的类型
    Type *type_res = Type::make_void_type();
    auto  kind     = root.get_kind();
    if (kind == "Identifier") {
        bool shot = false;
        for (auto iter = tables.rbegin(); iter != tables.rend(); ++iter) {
            auto symbol = iter->find_symbol(root.get_value());
            if (symbol != nullopt) {
                type_res = symbol.value().type;
                shot     = true;
                root.set_offset(symbol.value().offset);
                root.set_is_global(symbol.value().is_global);
                break;
            }
        }

        if (!shot) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Variable `" + root.get_value() + "` hasn't been declared in this scope.", 3);
            type_res = Type::make_error_type();
        }
        // END OF [Identifier]
    } else if (kind == "IndexExpr") {
        auto &children   = root.get_children();
        auto &array_root = children[0];
        auto  current    = expr(array_root);
        for (auto index = children.begin() + 1; index != children.end(); ++index) {
            auto index_type = expr(index->get_child(0));
            if (index_type->is_int()) {
                if (current->c_type != Ctype::Array) {
                    driver.report()
                        .report_level(Level::Error)
                        .report_loc(array_root.get_loc().value())
                        .report_msg("Type `" + current->to_string() + "` can not be indexed.", 4);
                    type_res = Type::make_error_type();
                    break;
                } else {
                    current  = current->base_type;
                    type_res = current;
                }
            } else {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(index->get_loc().value())
                    .report_msg("Array index should be integer.", 5);
                type_res = Type::make_error_type();
                break;
            }
        }
        // END OF [IndexExpr]
    } else if (kind == "ArithBinaryExpr") {
        // 算术二元运算，没有额外限制，表达式的结果由权值高的决定
        auto &a_node = root.get_child(0);
        auto  a_type = expr(a_node);

        auto &op = root.get_child(1);

        auto &b_node = root.get_child(2);
        auto  b_type = expr(b_node);

        if (!a_type->is_plain() || !b_type->is_plain()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Types around operand `" + op.get_value() + "` : `" + a_type->to_string() +
                        "`" + " and `" + b_type->to_string() +
                        "` can't perform this arithmetic calculate.",
                    6);
            type_res = Type::make_error_type();
        } else {
            type_res = upper_type(a_type, b_type);
        }
        // END OF [ArithBinaryExpr]
    } else if (kind == "LogBinaryExpr") {
        // 逻辑二元运算，必须是int类型
        auto &a_node = root.get_child(0);
        auto  a_type = expr(a_node);

        auto &op = root.get_child(1);

        auto &b_node = root.get_child(2);
        auto  b_type = expr(b_node);

        if (!a_type->is_int() || !b_type->is_int()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Types around logical operand `" + op.get_value() +
                        "` must both be `int`, but got : `" + a_type->to_string() + "`" + " and `" +
                        b_type->to_string() + "`.",
                    7);
            type_res = Type::make_error_type();
        } else {
            type_res = a_type;
        }
        // END OF [ArithBinaryExpr]
    } else if (kind == "RelationalExpr") {

        // 关系表达式，平凡类型随意比较，复合类型必须成员相同
        auto &a_node = root.get_child(0);
        auto  a_type = expr(a_node);

        auto &op = root.get_child(1);

        auto &b_node = root.get_child(2);
        auto  b_type = expr(b_node);

        if (!comparable(a_type, b_type)) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Types `" + a_type->to_string() + "` and `" + b_type->to_string() +
                        " is not comparable.",
                    8);
            type_res = Type::make_error_type();
        } else {
            type_res = Type::make_int_type();
        }
        // END OF [RelationalExpr]
    } else if (kind == "TernaryExpr") {
        // 三目表达式，条件具有int，分支取高来返回
        auto &cond_value = root.get_child(0);
        auto &yes_value  = root.get_child(1);
        auto &no_value   = root.get_child(2);

        auto cond_type = expr(cond_value);
        auto yes_type  = expr(yes_value);
        auto no_type   = expr(no_value);

        if (!cond_type->is_int()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Condition's type of ternary expression should be `int`, got: `" +
                        cond_type->to_string() + "`.",
                    9);
            type_res = Type::make_error_type();
        } else {
            auto upper_one = upper_type(yes_type, no_type);
            if (upper_one == nullptr) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(root.get_loc().value())
                    .report_msg(
                        "Branches' types of ternary expression not match, got `" +
                            yes_type->to_string() + "`" + " and `" + no_type->to_string() + "`.",
                        10);
                type_res = Type::make_error_type();
            } else {
                type_res = upper_one;
            }
        }
        // END OF [TernaryExpr]
    } else if (kind == "MemberExpr") {
        // 取成员操作符，a.b
        // a是一个结构体，b是a所属结构体中的一个成员
        auto &struct_node = root.get_child(0);
        auto  struct_type = expr(struct_node);
        auto  member_name = root.get_child(1).get_value();
        if (struct_type->c_type != Ctype::Struct) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Can't use member expresssion on non-struct value.", 11);
            type_res = Type::make_error_type();
        } else {
            if (struct_type->members.find(member_name) == struct_type->members.cend()) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(root.get_loc().value())
                    .report_msg(
                        "Struct type `" + struct_type->to_string() +
                            "` doesn't have member named `" + member_name + "`.",
                        12);
            } else {
                type_res = struct_type->members.at(member_name);
            }
        }
        // END OF [MemberExpr]
    } else if (kind == "CallExpr") {
        // 函数调用
        /*
            1. id为函数名
            2. 参数数量匹配
            3. 参数类型可转换
         */
        auto &func      = root.get_child(0);
        auto &args      = root.get_child(1).get_children();
        auto  func_type = expr(func);
        if (func_type->c_type != Ctype::Function) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(func.get_loc().value())
                .report_msg(
                    "Variable `" + func.get_value() + "` with type `" + func_type->to_string() +
                        "` is not callable.",
                    13);
            type_res = Type::make_error_type();
        } else {
            if (func_type->args.size() != args.size()) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(func.get_loc().value())
                    .report_msg(
                        "Function `" + func.get_value() + "` requires " +
                            to_string(func_type->args.size()) + " parameters, but " +
                            to_string(args.size()) + " is given.",
                        14);
                type_res = Type::make_error_type();
            } else {
                auto arg_iter   = args.begin();
                auto param_iter = func_type->args.begin();
                bool is_ok      = true;
                while (arg_iter != args.end()) {
                    auto arg_type = expr(*arg_iter);
                    if (!can_convert(arg_type, *param_iter)) {
                        driver.report()
                            .report_level(Level::Error)
                            .report_loc(arg_iter->get_loc().value())
                            .report_msg(
                                "Function `" + func.get_value() + "`requires type `" +
                                    (*param_iter)->to_string() + "` as parameter, but type `" +
                                    arg_type->to_string() + "` is given.",
                                15);
                        type_res = Type::make_error_type();
                        is_ok    = false;
                        break;
                    } else {
                        ++arg_iter;
                        ++param_iter;
                    }
                }
                if (is_ok) {
                    type_res = func_type->return_type;
                }
            }
        }
        // END OF [CallExpr]
    } else if (kind == "AssignmentExpr") {
        // 赋值语句 [=]
        auto &lhs = root.get_child(0);
        auto &rhs = root.get_child(2);

        auto lhs_type = expr(lhs);
        auto rhs_type = expr(rhs);
        if (!can_convert(rhs_type, lhs_type)) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(lhs.get_loc().value())
                .report_loc(rhs.get_loc().value())
                .report_msg(
                    "Type around assignment operator `=` is not assignable: `" +
                        lhs_type->to_string() + "` and `" + rhs_type->to_string() + "`.",
                    16);
            type_res = Type::make_error_type();
        } else {
            type_res = lhs_type;
        }
        // END OF [AssignmentExpr]
    } else if (kind == "CompoundArithAssignmentExpr") {
        // 算术结合赋值 [*= +=]
        auto &lhs = root.get_child(0);
        auto &op  = root.get_child(1);
        auto &rhs = root.get_child(2);

        auto lhs_type = expr(lhs);
        auto rhs_type = expr(rhs);
        if (!can_convert(rhs_type, lhs_type)) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(lhs.get_loc().value())
                .report_loc(rhs.get_loc().value())
                .report_msg(
                    "Type around assignment operator `" + op.get_value() +
                        "` is not assignable: `" + lhs_type->to_string() + "` and `" +
                        rhs_type->to_string() + ".",
                    17);
            type_res = Type::make_error_type();
        } else {
            type_res = lhs_type;
        }
        // END OF [CompoundArithAssignmentExpr]
    } else if (kind == "CompoundLogAssignmentExpr") {
        // 逻辑结合赋值 [|= &= ^=]
        auto &lhs = root.get_child(0);
        auto &op  = root.get_child(1);
        auto &rhs = root.get_child(2);

        auto lhs_type = expr(lhs);
        auto rhs_type = expr(rhs);

        if (!lhs_type->is_int() || !rhs_type->is_int()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(lhs.get_loc().value())
                .report_loc(rhs.get_loc().value())
                .report_msg(
                    "Type around logical compound assignment operator `" + op.get_value() +
                        "` should be `int`, but got `" + lhs_type->to_string() + "` and `" +
                        rhs_type->to_string() + ".",
                    18);
            type_res = Type::make_error_type();
        } else {
            if (!can_convert(rhs_type, lhs_type)) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(lhs.get_loc().value())
                    .report_loc(rhs.get_loc().value())
                    .report_msg(
                        "Type around assignment operator `" + op.get_value() +
                            "` is not assignable: `" + lhs_type->to_string() + "` and `" +
                            rhs_type->to_string() + ".",
                        19);
                type_res = Type::make_error_type();
            } else {
                type_res = lhs_type;
            }
        }
        // END OF [CompoundLogAssignmentExpr]
    } else if (kind == "ParenthesisExpr") {
        auto &inner = root.get_child(0);
        type_res    = expr(inner);
        // END OF [ParenthesisExpr]
    } else if (kind == "PrefixExpr") {
        auto &op   = root.get_child(0);
        auto &item = root.get_child(1);

        auto item_type = expr(item);
        auto op_str    = op.get_value();
        if (op_str == "-" || op_str == "+") {
            if (item_type->is_arith()) {
                type_res = item_type;
            } else {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(root.get_loc().value())
                    .report_msg(
                        "Operand of prefix expression `" + op_str +
                            "` should be arithmetic, but got `" + item_type->to_string() + "`",
                        20);
                type_res = Type::make_error_type();
            }
        } else { // --/++
            if (!item_type->is_int()) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(root.get_loc().value())
                    .report_msg(
                        "Operand of prefix expression `" + op_str + "` should be `int`, but got `" +
                            item_type->to_string() + "`",
                        21);
                type_res = Type::make_error_type();
            } else {
                type_res = item_type;
            }
        }
        // END OF [PrefixExpr]
    } else if (kind == "PostfixExpr") {
        auto &item = root.get_child(0);
        auto &op   = root.get_child(1);

        auto item_type = expr(item);
        auto op_str    = op.get_value();
        if (!item_type->is_int()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Operand of postfix expression `" + op_str + "` should be `int`, but got `" +
                        item_type->to_string() + "`",
                    22);
            type_res = Type::make_error_type();
        } else {
            type_res = item_type;
        }
        // END OF [PostfixExpr]
    }
    // FALL TO
    else if (
        kind == "IntegerLiteral" || kind == "FloatLiteral" || kind == "CharLiteral" ||
        kind == "StringLiteral") {
        type_res = Type::build_literal(root);
    } else {
        throw runtime_error("Uncaught Token Kind: `" + kind + "`.");
    }
    root.set_type(type_res);
    return type_res;
}

void Semantic::stmt(Token &root) {
    auto  kind     = root.get_kind();
    auto &children = root.get_children();
#ifdef DEBUG
    cout << "Walk : `" << kind << "`" << endl;
#endif

    if (kind == "Program") {
        // 程序的根，从这里开始，建立一个根符号表，别名为GLOBAL
        enter_scope("GLOBAL");
        for (auto &child : children) {
            stmt(child);
        }
        root.set_func_stack_size(global_stack_size);
        leave_scope(global_stack_size);
    } else if (kind == "BlockStmt") {
        // 块级语句，可能是新的作用域
        // 有别于函数体、循环体的块，这俩单独处理，不在这里搞
        enter_scope("BLOCK");
        for (auto &child : children) {
            stmt(child);
        }
        leave_scope(local_stack_size);
    } else if (kind == "Struct") {
        // 声明Struct类型，需要Identifier与Struct体，通过名字收录
        if (children.empty()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Incomplete struct type `" + root.get_value() + "`.", 23);
            return;
        }
        if (type_table.find(root.get_value()) != type_table.end()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Struct type `" + root.get_value() + "` has already been declared.", 24);
            return;
        }
        auto struct_type = Type::build_type(root, {}, type_table);
        type_table.insert({root.get_value(), struct_type});
    } else if (kind == "GlobalVarDecl" || kind == "LocalVarDecl") {
        // [声明/定义][全局/局部]变量
        // 0: <Type>
        // 1: <VarList>
        // VarList -> {VarDecl} | {VarDef}
        //

        // [构建类型] -> [构建数组尺度] -> [构建符号] -> [统计全局/局部增长]
        auto &type_node = root.get_child(0);

        Type *type_res = parse_type(type_node);

        // VarList
        auto &ident_list = root.get_child(1).get_children();
        for (auto &ident_node_wrap : ident_list) {
            // if (ident_node_wrap.get_kind() == "VarDecl") {

            // 可能有 VarDecl VarDef
            // VarDef需要进行赋值，但也需要定义类型与插入符号表，因此合并公操作
            auto &ident_node = ident_node_wrap.get_child(0);
            Token name;
            if (ident_node.get_kind() == "IndexExpr") {
                // 为数组
                name        = ident_node.get_child(0);
                auto depths = Type::make_array_depths(ident_node);
                type_res    = Type::wrap_array(type_res, depths);
            } else if (ident_node.get_kind() == "Identifier") {
                // 为普通标识符
                name = ident_node;
            }

            if (kind == "GlobalVarDecl") {
                global_stack_size = align_memory(global_stack_size, type_res->align);
                // 插入符号表 and 全局偏移
                try_insert_symbol(name, type_res, true, global_stack_size);
                global_stack_size += type_res->size;
            } else if (kind == "LocalVarDecl") {
                local_stack_size = align_memory(local_stack_size, type_res->align);
                try_insert_symbol(name, type_res, false, local_stack_size);
                ident_node_wrap.set_offset(local_stack_size);
                local_stack_size += type_res->size;
            }

            expr(ident_node);

            if (ident_node_wrap.get_kind() == "VarDef") {
                auto &lhs = ident_node;
                auto &rhs = ident_node_wrap.get_child(2);

                auto lhs_type = expr(lhs);
                auto rhs_type = expr(rhs);
                if (!can_convert(rhs_type, lhs_type)) {
                    driver.report()
                        .report_level(Level::Error)
                        .report_loc(lhs.get_loc().value())
                        .report_loc(rhs.get_loc().value())
                        .report_msg(
                            "Type around assignment operator `=` is not assignable: `" +
                                lhs_type->to_string() + "` and `" + rhs_type->to_string() + "`.",
                            16);
                }
            }
        }
        // END OF [GlobalVarDecl] AND [LocalVarDecl]
    } else if (kind == "FuncDef") {
        // 函数定义，有实现的代码块部分
        /*
         * 0: 返回类型节点 <Type> | <Struct>
         * 1: 函数名<Identifier>
         * 2: 参数列表 <ParamList> -> {ParamDecl}
         * 3: 函数体 {BlockStmt}
         */
        inner_return_ty = Type::make_void_type();

        // 1. 函数签名
        auto &name_node = root.get_child(1);

        auto func_type = Type::build_type(root, {}, type_table);
        try_insert_symbol(name_node, func_type, true);

        enter_scope("F(" + name_node.get_value() + ")");

        // 2. 参数列表作为局部定义变量
        auto &params = root.get_child(2).get_children();
        for (auto &param : params) {
            auto  param_type = parse_type(param.get_child(0));
            auto &param_name = param.get_child(1);

            Token name;
            if (param_name.get_kind() == "IndexExpr") {
                // 为数组
                name        = param_name.get_child(0);
                auto depths = Type::make_array_depths(param_name);
                param_type  = Type::wrap_array(param_type, depths);
            } else if (param_name.get_kind() == "Identifier") {
                // 为普通标识符
                name = param_name;
            }

            local_stack_size = align_memory(local_stack_size, param_type->align);
            try_insert_symbol(name, param_type, false, local_stack_size);
            local_stack_size += param_type->size;
        }

        // 函数体
        for (auto &child : root.get_child(3).get_children()) {
            stmt(child);
        }
        leave_scope(local_stack_size);
        root.set_func_stack_size(local_stack_size);
        // 退出函数后需要将局部栈清空，也就是这个RBP的寿命结束了
        local_stack_size = 0;

        // 返回值类型检查
        auto ret_type = func_type->return_type;
        if (!can_convert(inner_return_ty, ret_type)) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_child(0).get_loc().value())
                .report_msg(
                    "Return type not match: Function `" + name_node.get_value() +
                        "` requires return type: `" + ret_type->to_string() + "`, but `" +
                        (inner_return_ty == nullptr ? "void" : inner_return_ty->to_string()) +
                        "` was given.",
                    25);
        }
        // END OF [FuncDef]
    } else if (kind == "ReturnStmt") {
        // 返回语句，return <Expr>
        /*
         * Expr具有一个类型，这个类型赋值给inner_return_ty，
         * 然后在函数结束的位置与函数预期的类型进行比较
         */
        if (!root.get_children().empty()) {
            inner_return_ty = expr(root.get_child(0));
        }
        // END OF [ReturnStmt]
    } else if (kind == "ContinueStmt") {
        // continue语句，必须在循环之内
        if (!loops) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Keyword `continue` should stay in loop blocks.", 26);
        }
        // END OF [ContinueStmt]
    } else if (kind == "BreakStmt") {
        // 同上
        if (!loops) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Keyword `break` should stay in loop blocks.", 27);
        }
        // END OF [BreakStmt]
    } else if (kind == "WhileStmt") {
        // while 语句
        // 0: <Expr> 条件
        // 1: <Stmt>

        ++loops;
        for (auto &child : root.get_children()) {
            stmt(child);
        }
        --loops;
        // END OF [WhileStmt]
    } else if (kind == "WhileStmt" || kind == "DoWhileStmt") {
        // do-while 语句
        // 0: <Stmt>
        // 1: <Expr>

        ++loops;
        for (auto &child : root.get_children()) {
            stmt(child);
        }
        --loops;
        // END OF [DoWhileStmt]
    } else if (kind == "ForStmt") {
        // for循环语句
        /// FIXME 细化
        ++loops;
        for (auto &child : root.get_children()) {
            stmt(child);
        }
        --loops;
    } else if (kind == "IfStmt") {
        for (auto &child : root.get_children()) {
            stmt(child);
        }
    } else if (kind == "ElseStmt") {
        for (auto &child : root.get_children()) {
            stmt(child);
        }
    } else {
        expr(root);
    }
}

Type *Semantic::parse_type(Token &root) {
    Type *type_res = nullptr;
    if (root.get_kind() == "Struct") {
        // 声明结构体类型
        /*
         * 1. 匿名结构体 -> 建立类型后直接插入符号表，返回类型
         * 2. 具名结构体 ->
         *      a. 没有结构体成员部分 -> 查找类型表，返回类型
         *      b. 有结构体成员部分   -> 插入类型表，返回类型
         */
        if (root.get_value().empty()) {
            // 匿名结构体
            type_res = Type::build_type(root, {}, type_table);
        } else if (root.get_children().empty()) {
            // 具名 且 无成员定义
            if (type_table.find(root.get_value()) == type_table.end()) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(root.get_loc().value())
                    .report_msg(
                        "Struct type `" + root.get_value() + "` is used before declaration.", 28);
                type_res = Type::make_error_type();
            } else {
                type_res = type_table.at(root.get_value());
            }
        } else {
            // 具名 且 有成员定义
            stmt(root);
            type_res = type_table.at(root.get_value());
        }
    } else if (root.get_kind() == "Type") {
        // 平凡类型
        type_res = Type::build_type(root, {}, type_table);
    } else {
        throw runtime_error("Uncaught Kind : `" + root.get_kind() + "`.");
    }
    return type_res;
}

// 判断能否从类型from转换到类型to
bool Semantic::can_convert(Type *from, Type *to) {
    /*
     *  Char -> Int
     *  Int -> Float
     */
    if ((from != nullptr && to == nullptr) || (from == nullptr && to != nullptr))
        return false;
    if (from->c_type == Ctype::Function) {
        return false;
    }
    if (from->c_type != to->c_type) {
        return false;
    }
    if (from->c_type == Ctype::Array) {
        return from->length == to->length && can_convert(from->base_type, to->base_type);
    }
    if (from->c_type == Ctype::Struct) {
        if (from->members.size() != to->members.size()) {
            return false;
        }
        auto from_iter = from->members.cbegin();
        auto to_iter   = to->members.cbegin();
        while (from_iter != from->members.cend()) {
            if (!can_convert(from_iter->second, to_iter->second))
                return false;
            ++from_iter;
            ++to_iter;
        }
        return true;
    }
    if (from->is_plain()) {
        if (from->plain_type == to->plain_type) {
            return true;
        }
        if (from->plain_type == PlainType::Void || to->plain_type == PlainType::Void) {
            return false;
        }
        return from->plain_type <= to->plain_type;
    }
    return false;
}

// 给出两个类型中，高的一个
/*
    1. char -> int -> float
    2. 结构体与数组不存在高低，必须相等
 */
Type *Semantic::upper_type(Type *a, Type *b) {
    if (a->is_plain() && b->is_plain()) {
        if (a->plain_type > b->plain_type) {
            return a;
        } else {
            return b;
        }
    } else {
        if (can_convert(a, b)) {
            return a;
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

// 两个是否可比
bool Semantic::comparable(Type *a, Type *b) {
    if ((a != nullptr && b == nullptr) || (a == nullptr && b != nullptr))
        return false;
    if (a->c_type == Ctype::Function || b->c_type == Ctype::Function) {
        return false;
    }
    if (a->c_type != b->c_type) {
        return false;
    }
    if (a->is_plain())
        return true;
    if (a->c_type == Ctype::Array) {
        return a->length == b->length && comparable(a->base_type, b->base_type);
    }
    if (a->c_type == Ctype::Struct) {
        if (a->members.size() != b->members.size()) {
            return false;
        }
        auto a_iter = a->members.cbegin();
        auto b_iter = b->members.cbegin();
        while (a_iter != a->members.cend()) {
            if (!can_convert(a_iter->second, b_iter->second))
                return false;
            ++a_iter;
            ++b_iter;
        }
        return true;
    }
    return false;
}