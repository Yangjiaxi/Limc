#include "3-analyzer.h"
#include "0-driver.h"
#include "util.h"

#define DEBUG

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

void Semantic::leave_scope() {
#ifdef DEBUG
    cout << "<<< Leave Scope\t: [" << tables.back().get_alias() << "]" << endl;
#endif
    tables.pop_back();
    current_table = &tables.back();
}

void Semantic::try_insert_symbol(Token &identifier, Type *type, unsigned offset) {
    auto already_exist = current_table->insert_symbol(Symbol(identifier.get_value(), type, offset));
    if (already_exist) {
        driver.report()
            .report_level(Level::Error)
            .report_loc(identifier.get_loc().value())
            .report_msg("Symbol `" + identifier.get_value() + "` has already been declared. ");
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
                break;
            }
        }
        if (!shot) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg(
                    "Variable `" + root.get_value() + "` hasn't been declared in this scope.");
            type_res = Type::make_error_type();
        }
        // END OF [Identifier]
    } else if (kind == "IndexExpr") {
        cout << "IndexExpr" << endl;
        auto children   = root.get_children();
        auto array_root = children[0];
        auto current    = expr(array_root);
        for (auto index = children.begin() + 1; index != children.end(); ++index) {
            auto index_type = expr(index->get_child(0));
            if (index_type->c_type == Ctype::Plain && index_type->plain_type == PlainType::Int) {
                if (current->c_type != Ctype::Array) {
                    driver.report()
                        .report_level(Level::Error)
                        .report_loc(array_root.get_loc().value())
                        .report_msg("Type `" + current->to_string() + "` can not be indexed.");
                    type_res = Type::make_error_type();
                    break;
                } else {
                    current = current->base_type;
                }
            } else {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(index->get_loc().value())
                    .report_msg("Array index should be integer.");
                type_res = Type::make_error_type();
                break;
            }
        }
        type_res = current;
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
        leave_scope();
    } else if (kind == "BlockStmt") {
        // 块级语句，可能是新的作用域
        // 有别于函数体、循环体的块，这俩单独处理，不在这里搞
        enter_scope("BLOCK");
        for (auto &child : children) {
            stmt(child);
        }
        leave_scope();
    } else if (kind == "Struct") {
        // 声明Struct类型，需要Identifier与Struct体，通过名字收录
        if (children.empty()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Incomplete struct type `" + root.get_value() + "`.");
            return;
        }
        if (type_table.find(root.get_value()) != type_table.end()) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Struct type `" + root.get_value() + "` has already been declared.");
            return;
        }
        auto struct_type = Type::build_type(root);
        type_table.insert({root.get_value(), struct_type});
    } else if (kind == "GlobalVarDecl" || kind == "LocalVarDecl") {
        // [声明/定义][全局/局部]变量
        // 0: <Type>
        // 1: <VarList>
        // VarList -> {VarDecl} | {VarDef}
        //

        // [构建类型] -> [构建数组尺度] -> [构建符号] -> [统计全局/局部增长]
        auto type_node = root.get_child(0);

        Type *type_res = parse_type(type_node);

        // VarList
        auto ident_list = root.get_child(1).get_children();
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
                try_insert_symbol(name, type_res, global_stack_size);
                global_stack_size += type_res->size;
            } else if (kind == "LocalVarDecl") {
                // cout << "Insert " << name.get_value() << endl;
                local_stack_size = align_memory(local_stack_size, type_res->align);
                try_insert_symbol(name, type_res, local_stack_size);
                local_stack_size += type_res->size;
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
        auto  func_type = Type::build_type(root);
        cout << name_node.get_value() << " : " << func_type->to_string() << endl;
        try_insert_symbol(name_node, func_type);

        enter_scope("F(" + name_node.get_value() + ")");

        // 2. 参数列表作为局部定义变量
        auto &params = root.get_child(2).get_children();
        for (auto &param : params) {
            auto param_type = parse_type(param.get_child(0));
            auto param_name = param.get_child(1);

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
            try_insert_symbol(name, param_type, local_stack_size);
            local_stack_size += param_type->size;
        }

        // 函数体
        for (auto &child : root.get_child(3).get_children()) {
            stmt(child);
        }
        leave_scope();
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
                    "` was given.");
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
            cout << "Inner : " << inner_return_ty->to_string() << endl;
        }
        // END OF [ReturnStmt]
    } else if (kind == "ContinueStmt") {
        // continue语句，必须在循环之内
        if (!loops) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Keyword `continue` should stay in loop blocks.");
        }
        // END OF [ContinueStmt]
    } else if (kind == "BreakStmt") {
        // 同上
        if (!loops) {
            driver.report()
                .report_level(Level::Error)
                .report_loc(root.get_loc().value())
                .report_msg("Keyword `break` should stay in loop blocks.");
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
            type_res = Type::build_type(root);
        } else if (root.get_children().empty()) {
            // 具名 且 无成员定义
            if (type_table.find(root.get_value()) == type_table.end()) {
                driver.report()
                    .report_level(Level::Error)
                    .report_loc(root.get_loc().value())
                    .report_msg(
                        "Struct type `" + root.get_value() + "` is used before declaration.");
                return nullptr;
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
        type_res = Type::build_type(root);
    } else {
        throw runtime_error("Uncaught Kind : `" + root.get_kind() + "`.");
    }
    return type_res;
}

// 判断能否总类型to转换到类型from
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
        return can_convert(from->base_type, to->base_type);
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
    if (from->c_type == Ctype::Plain) {
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

// Type Semantic::check_type(Token &root) { return Type(); }
