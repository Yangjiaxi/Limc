#include "4-gen_ir.h"

#include "0-driver.h"
#include "util.h"

#define DEBUG

using namespace Limc;
using namespace std;

GenIR::GenIR(Driver &driver) : driver(driver), ir_box() {}

string GenIR::gen_expr(Token &root) {
    auto  kind     = root.get_kind();
    auto &children = root.get_children();
#ifdef DEBUG
    cout << "Gen Expr : `" << kind << "`" << endl;
#endif
    if (kind == "Identifier") {
        return "@" + root.get_value();
        // END OF [Identifier]
    } else if (kind == "IndexExpr") {

        // END OF [IndexExpr]
    } else if (kind == "ArithBinaryExpr") {
        auto tmp = generate_temp();
        auto lhs = gen_expr(children[0]);
        auto rhs = gen_expr(children[2]);
        auto op  = children[1].get_value();
        ir_box.append().set_op(op).set_arg1(lhs).set_arg2(rhs).set_res(tmp);
        return tmp;
        // END OF [ArithBinaryExpr]
    } else if (kind == "LogBinaryExpr") {
        auto op = children[1].get_value();
        if (op == "&&") {

        } else if (op == "||") {

        } else {
            auto tmp = generate_temp();
            auto lhs = gen_expr(children[0]);
            auto rhs = gen_expr(children[2]);

            ir_box.append().set_op(op).set_arg1(lhs).set_arg2(rhs).set_res(tmp);
            return tmp;
        }
        // END OF [ArithBinaryExpr]
    } else if (kind == "RelationalExpr") {
        auto tmp = generate_temp();
        auto lhs = gen_expr(children[0]);
        auto rhs = gen_expr(children[2]);
        auto op  = children[1].get_value();
        ir_box.append().set_op(op).set_arg1(lhs).set_arg2(rhs).set_res(tmp);
        return tmp;
        // END OF [RelationalExpr]
    } else if (kind == "TernaryExpr") {
        auto tmp = generate_temp();

        auto false_label = generate_label();
        auto end_label   = generate_label();

        auto cond = gen_expr(children[0]);
        ir_box.append().set_op("jz").set_arg1(cond).set_res(false_label);

        auto true_res = gen_expr(children[2]);
        ir_box.append().set_op("=").set_arg1(true_res).set_res(tmp);
        ir_box.append().set_op("goto").set_res(end_label);
        ir_box.append().set_op("label").set_res(false_label);

        auto false_res = gen_expr(children[4]);
        ir_box.append().set_op("=").set_arg1(false_res).set_res(tmp);
        ir_box.append().set_op("label").set_res(end_label);
        return tmp;
        // END OF [TernaryExpr]
    } else if (kind == "MemberExpr") {

        // END OF [MemberExpr]
    } else if (kind == "CallExpr") {
        auto tmp  = generate_temp();
        auto args = children[1].get_children();
        // 反向压入参数
        for (auto arg = args.rbegin(); arg != args.rend(); arg++) {
            auto inner = gen_expr(*arg);
            ir_box.append().set_op("push").set_arg1(inner);
        }
        ir_box.append().set_op("call").set_arg1(children[0].get_value()).set_res(tmp);
        for (auto &arg : args) {
            ir_box.append().set_op("pop");
        }
        return tmp;
        // END OF [CallExpr]
    } else if (kind == "AssignmentExpr") {
        auto lhs = gen_expr(children[0]);
        auto rhs = gen_expr(children[2]);
        ir_box.append().set_op("=").set_arg1(rhs).set_res(lhs);
        // END OF [AssignmentExpr]
    } else if (kind == "CompoundArithAssignmentExpr") {
        auto lhs = gen_expr(children[0]);
        auto rhs = gen_expr(children[2]);
        auto op  = children[1].get_value();
        ir_box.append()
            .set_op(op.substr(0, op.length() - 1))
            .set_arg1(rhs)
            .set_arg2(lhs)
            .set_res(lhs);
        // END OF [CompoundArithAssignmentExpr]
    } else if (kind == "CompoundLogAssignmentExpr") {
        auto lhs = gen_expr(children[0]);
        auto rhs = gen_expr(children[2]);
        auto op  = children[1].get_value();
        ir_box.append()
            .set_op(op.substr(0, op.length() - 1))
            .set_arg1(rhs)
            .set_arg2(lhs)
            .set_res(lhs);
        // END OF [CompoundLogAssignmentExpr]
    } else if (kind == "ParenthesisExpr") {
        return gen_expr(children[0]);
        // END OF [ParenthesisExpr]
    } else if (kind == "PrefixExpr") {
        auto tmp    = generate_temp();
        auto op     = children[0].get_value();
        auto target = gen_expr(children[1]);
        ir_box.append().set_op(op).set_arg1(target).set_res(tmp);
        return tmp;
        // END OF [PrefixExpr]
    } else if (kind == "PostfixExpr") {
        auto tmp    = generate_temp();
        auto op     = children[1].get_value();
        auto target = gen_expr(children[0]);
        ir_box.append().set_op(op).set_arg1(target).set_res(tmp);
        return tmp;
        // END OF [PostfixExpr]
    } else if (
        kind == "IntegerLiteral" || kind == "FloatLiteral" || kind == "CharLiteral" ||
        kind == "StringLiteral") {
        /// TODO
        return root.get_value();
    } else {
        throw runtime_error("Uncaught Token Kind: `" + kind + "`.");
    }

    return "";
}

string GenIR::gen_stmt(Token &root) {
    auto  kind     = root.get_kind();
    auto &children = root.get_children();

#ifdef DEBUG
    cout << "Gen Stmt : `" << kind << "`" << endl;
#endif

    if (kind == "Program") {

        for (auto &child : children) {
            gen_stmt(child);
        }
        // END OF [Program]
    } else if (kind == "BlockStmt") {

        for (auto &child : children) {
            gen_stmt(child);
        }
        // END OF [BlockStmt]
    } else if (kind == "Struct") {
        // PASS
        // END OF [Struct]
    } else if (kind == "GlobalVarDecl" || kind == "LocalVarDecl") {
        auto &ident_list = root.get_child(1).get_children();
        for (auto &ident_node_wrap : ident_list) {
            if (ident_node_wrap.get_kind() == "VarDef") {
                auto &lhs   = ident_node_wrap.get_child(0);
                auto &rhs   = ident_node_wrap.get_child(2);
                auto  left  = gen_expr(lhs);
                auto  right = gen_expr(rhs);
                ir_box.append().set_op("=").set_arg1(right).set_res(left);
            }
        }
        // END OF [GlobalVarDecl] AND [LocalVarDecl]
    } else if (kind == "FuncDef") {
        ir_box.append().set_op("start").set_res(children[1].get_value());
        auto func_end_label  = generate_label();
        current_return_label = func_end_label;
        for (auto &child : root.get_child(3).get_children()) {
            gen_stmt(child);
        }
        ir_box.append().set_op("label").set_res(func_end_label);
        ir_box.append().set_op("end").set_res(children[1].get_value());
        // END OF [FuncDef]
    } else if (kind == "ReturnStmt") {
        string expr;
        if (!children.empty())
            expr = gen_expr(children[0]);
        ir_box.append().set_op("ret").set_res(expr);
        ir_box.append().set_op("goto").set_res(current_return_label);
        // END OF [ReturnStmt]
    } else if (kind == "ContinueStmt") {
        ir_box.append().set_op("goto").set_res(current_continue_labels.back());
        // END OF [ContinueStmt]
    } else if (kind == "BreakStmt") {
        ir_box.append().set_op("goto").set_res(current_break_labels.back());
        // END OF [BreakStmt]
    } else if (kind == "WhileStmt") {
        auto start_label = generate_label();
        auto end_label   = generate_label();
        current_continue_labels.push_back(start_label);
        current_break_labels.push_back(end_label);

        ir_box.append().set_op("label").set_res(start_label);
        ir_box.append().set_op("jz").set_arg1(gen_expr(children[0])).set_res(end_label);
        gen_stmt(children[1]); // while body
        ir_box.append().set_op("goto").set_res(start_label);
        ir_box.append().set_op("label").set_res(end_label);

        current_continue_labels.pop_back();
        current_break_labels.pop_back();
        // END OF [WhileStmt]
    } else if (kind == "DoWhileStmt") {
        auto current_index = ir_box.size();
        ir_box.append().set_op("label");

        gen_stmt(children[0]);

        auto start_label = generate_label();
        auto end_label   = generate_label();
        current_continue_labels.push_back(start_label);
        current_break_labels.push_back(end_label);

        auto cond = gen_expr(children[1]);
        ir_box.append().set_op("jnz").set_arg1(cond).set_res(start_label);
        ir_box[current_index].set_res(start_label);
        ir_box.append().set_op("label").set_res(end_label);

        current_continue_labels.pop_back();
        current_break_labels.pop_back();
        // END OF [DoWhileStmt]
    } else if (kind == "ForStmt") {
        gen_expr(children[0]); // init

        auto start_label = generate_label();
        auto end_label   = generate_label();
        current_continue_labels.push_back(start_label);
        current_break_labels.push_back(end_label);

        ir_box.append().set_op("label").set_res(start_label);

        auto cond = gen_expr(children[1]);
        ir_box.append().set_op("jz").set_arg1(cond).set_res(end_label); // cond

        gen_stmt(children[3]); // body
        gen_expr(children[2]); // step

        ir_box.append().set_op("goto").set_res(start_label);
        ir_box.append().set_op("label").set_res(end_label);

        current_continue_labels.pop_back();
        current_break_labels.pop_back();
        // END OF [ForStmt]
    } else if (kind == "IfStmt") {
        auto label = generate_label();
        auto cond  = gen_stmt(children[0]);
        ir_box.append().set_op("jz").set_arg1(cond).set_res(label);
        gen_stmt(children[1]);      // body
        if (children.size() == 3) { // if-else if
            auto end_label = generate_label();
            ir_box.append().set_op("goto").set_res(end_label);
            ir_box.append().set_op("label").set_res(label);
            for (auto &child : children[2].get_children()) {
                gen_stmt(child);
            }
            ir_box.append().set_op("label").set_res(end_label);
        } else {
            ir_box.append().set_op("label").set_res(label);
        }
        // END OF [IfStmt]
    } else {
        gen_expr(root);
    }

    return "";
}

string GenIR::generate_temp() { return ".temp" + to_string(current_temp++); }

string GenIR::generate_label() { return ".L" + to_string(current_label++); }

void GenIR::show_irs() { ir_box.show(); }
