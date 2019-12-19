#include "4-gen_ir.h"

#include "0-driver.h"
#include "util.h"

#define DEBUG
// #define DEBUG_INPLACE

using namespace Limc;
using namespace std;
using opt_uint = optional<unsigned>;

GenIR::GenIR(Driver &driver) : driver(driver), funcs(), codes() {}

opt_uint GenIR::gen_expr(Token &node) {
    auto  kind     = node.get_kind();
    auto &children = node.get_children();
#ifdef DEBUG
    cout << "Gen Expr : `" << kind << "`" << endl;
#endif
#ifdef DEBUG_INPLACE
    comment(kind);
#endif
    if (kind == "Identifier") {
        auto reg = gen_left_value(node);
        load(node.get_type()->size, reg, reg);
        return reg;
        // END OF [Identifier]
    } else if (kind == "IndexExpr") {
        auto reg = gen_left_value(node);
        load(node.get_type()->size, reg, reg);
        return reg;
        // END OF [IndexExpr]
    } else if (kind == "ArithBinaryExpr") {
        // + | - | * | / | %
        auto &lhs = children[0];
        auto &rhs = children[2];
        auto  op  = children[1].get_value();
        if (op == "+") {
            return gen_binary_op(IROp::Add, lhs, rhs);
        } else if (op == "-") {
            return gen_binary_op(IROp::Sub, lhs, rhs);
        } else if (op == "*") {
            return gen_binary_op(IROp::Mul, lhs, rhs);
        } else if (op == "/") {
            return gen_binary_op(IROp::Div, lhs, rhs);
        } else if (op == "%") {
            return gen_binary_op(IROp::Mod, lhs, rhs);
        }
        // END OF [ArithBinaryExpr]
    } else if (kind == "LogBinaryExpr") {
        // `&&` | `||` | `&` | `|` | `^` | `>>` | `<<`
        auto &lhs = children[0];
        auto &rhs = children[2];
        auto  op  = children[1].get_value();
        if (op == "&&") {
            auto end_label = new_label();

            auto reg1 = gen_expr(lhs);
            add_ir(IROp::Unless, reg1, end_label);

            auto reg2 = gen_expr(rhs);
            add_ir(IROp::Mov, reg1, reg2);
            kill(reg2);

            add_ir(IROp::Unless, reg1, end_label);
            add_ir(IROp::Imm, reg1, 1);

            label(end_label);
            return reg1;
        } else if (op == "||") {
            auto second_start = new_label();
            auto end_label    = new_label();

            auto reg1 = gen_expr(lhs);
            add_ir(IROp::Unless, reg1, second_start);
            add_ir(IROp::Imm, reg1, 1);
            jmp(end_label);

            label(second_start);
            auto reg2 = gen_expr(rhs);
            add_ir(IROp::Mov, reg1, reg2);
            kill(reg2);
            add_ir(IROp::Unless, reg1, end_label);
            add_ir(IROp::Imm, reg1, 1);

            label(end_label);
            return reg1;
        } else if (op == "&") {
            return gen_binary_op(IROp::And, lhs, rhs);
        } else if (op == "|") {
            return gen_binary_op(IROp::Or, lhs, rhs);
        } else if (op == "^") {
            return gen_binary_op(IROp::Xor, lhs, rhs);
        } else if (op == "<<") {
            return gen_binary_op(IROp::Shl, lhs, rhs);
        } else if (op == ">>") {
            return gen_binary_op(IROp::Shr, lhs, rhs);
        }
        // END OF [ArithBinaryExpr]
    } else if (kind == "RelationalExpr") {
        // ">"|"<"|">="|"<="|"=="|"!="
        auto &lhs = children[0];
        auto &rhs = children[2];
        auto  op  = children[1].get_value();
        if (op == ">") {
            return gen_binary_op(IROp::GT, lhs, rhs);
        } else if (op == "<") {
            return gen_binary_op(IROp::LT, lhs, rhs);
        } else if (op == ">=") {
            return gen_binary_op(IROp::GE, lhs, rhs);
        } else if (op == "<=") {
            return gen_binary_op(IROp::LE, lhs, rhs);
        } else if (op == "==") {
            return gen_binary_op(IROp::EQ, lhs, rhs);
        } else if (op == "!=") {
            return gen_binary_op(IROp::NE, lhs, rhs);
        }
        // END OF [RelationalExpr]
    } else if (kind == "TernaryExpr") {
        auto &cond_node = node.get_child(0);
        auto &then_node = node.get_child(1);
        auto &else_node = node.get_child(2);

        auto second_start = new_label();
        auto end_label    = new_label();

        auto reg = gen_expr(cond_node);

        add_ir(IROp::Unless, reg, second_start);
        auto then_reg = gen_expr(then_node);
        add_ir(IROp::Mov, reg, then_reg);
        kill(then_reg);
        jmp(end_label);

        label(second_start);
        auto else_reg = gen_expr(else_node);
        add_ir(IROp::Mov, reg, else_reg);
        kill(else_reg);
        label(end_label);
        return reg;

        // END OF [TernaryExpr]
    } else if (kind == "MemberExpr") {
        auto reg = gen_left_value(node);
        load(node.get_type()->size, reg, reg);
        return reg;
        // END OF [MemberExpr]
    } else if (kind == "CallExpr") {

        // END OF [CallExpr]
    } else if (kind == "AssignmentExpr") {
        auto rhs = gen_expr(children[2]);
        auto lhs = gen_left_value(children[0]);
        store(node.get_type()->size, lhs, rhs);
        kill(lhs);
        return rhs;
        // END OF [AssignmentExpr]
    } else if (kind == "CompoundArithAssignmentExpr") {
        // "+="|"-="|"*="|"/="|"%="
        auto &lhs = children[0];
        auto &rhs = children[2];
        auto  op  = convert_compound_op(children[1]);
        return gen_comp_assign(op, node.get_type()->size, lhs, rhs);
        // END OF [CompoundArithAssignmentExpr]
    } else if (kind == "CompoundLogAssignmentExpr") {
        // "&="|"|="|"^="|"<<="|">>="
        auto &lhs = children[0];
        auto &rhs = children[2];
        auto  op  = convert_compound_op(children[1]);
        return gen_comp_assign(op, node.get_type()->size, lhs, rhs);
        // END OF [CompoundLogAssignmentExpr]
    } else if (kind == "ParenthesisExpr") {
        return gen_expr(children[0]);
        // END OF [ParenthesisExpr]
    } else if (kind == "PrefixExpr") {
        // - | ! | ++ | --
        auto  op   = children[0].get_value();
        auto &item = children[1];
        if (op == "-") {
            // NEG
            auto reg = gen_expr(item);
            add_ir(IROp::Neg, reg, nullopt);
            return reg;
        } else if (op == "!") {
            // 逻辑非
            auto res = gen_expr(item);
            auto tmp = new_reg();
            add_ir(IROp::Imm, tmp, 0);
            add_ir(IROp::EQ, res, tmp);
            kill(tmp);
            return res;
        } else if (op == "++") {
            // 前置自增
        } else if (op == "--") {
            // 前置自减
        }
        // END OF [PrefixExpr]
    } else if (kind == "PostfixExpr") {

        // END OF [PostfixExpr]
    } else if (kind == "IntegerLiteral") {
        auto reg = new_reg();
        add_ir(IROp::Imm, reg, stoi(node.get_value()));
        return reg;
        // END OF [IntegerLiteral]
    } else if (kind == "FloatLiteral" || kind == "CharLiteral" || kind == "StringLiteral") {
        /// TODO
    } else {
        throw runtime_error("Uncaught Token Kind: `" + kind + "`.");
    }
    return nullopt;
}

void GenIR::gen_stmt(Token &node) {
    auto  kind     = node.get_kind();
    auto &children = node.get_children();
#ifdef DEBUG
    cout << "Gen for `" << kind << "`." << endl;
#endif
#ifdef DEBUG_INPLACE
    comment(kind);
#endif
    if (kind == "LocalVarDecl") {
        for (auto &vardef : children) {
            auto &group = vardef.get_children();
            if (group.size() == 2) {
                // 定义时赋值
                auto rhs = gen_expr(group[1]);
                auto lhs = new_reg();
                add_ir(IROp::BpOffset, lhs, group[0].get_offset());
                store(group[0].get_type()->size, lhs, rhs);
                kill(lhs);
                kill(rhs);
            }
        }
        // END OF [LocalVarDecl]
    } else if (kind == "BlockStmt") {
        for (auto &child : children) {
            gen_stmt(child);
        }
        // END OF [BlockStmt]
    } else if (kind == "ReturnStmt") {

        // END OF [ReturnStmt]
    } else if (kind == "ContinueStmt") {

        // END OF [ContinueStmt]
    } else if (kind == "BreakStmt") {

        // END OF [BreakStmt]
    } else if (kind == "WhileStmt") {

        // END OF [WhileStmt]
    } else if (kind == "DoWhileStmt") {

        // END OF [DoWhileStmt]
    } else if (kind == "ForStmt") {

        // END OF [ForStmt]
    } else if (kind == "IfStmt") {

        // END OF [IfStmt]
    }
    // Fall to Expr
    else {
        auto reg = gen_expr(node);
        kill(reg);
    }
}

vector<BasicFunc> GenIR::gen_ir(Token &node) {
    // TODO
    for (auto &block : node.get_children()) {

        auto kind = block.get_kind();
        if (kind == "FuncDef") {
            codes.clear();

            auto  name   = block.get_child(1).get_value();
            auto  stk_sz = block.get_func_stack_size();
            auto &params = block.get_child(2).get_children();
            auto &body   = block.get_child(3);

            for (unsigned long i = 0; i < params.size(); ++i) {
                auto &node = params[i];
                cout << node.get_value() << endl;
                store_param(node.get_type()->size, node.get_offset(), i);
            }

            gen_stmt(body);

            funcs.emplace_back(move(BasicFunc(name, codes, stk_sz)));
            funcs.back().print_ir();
            // auto &
        } else {
#ifdef DEBUG
            cout << "Pass `" << kind << "`." << endl;
#endif
        }
    }
    return move(funcs);
}

void GenIR::store_param(unsigned size, opt_uint bp_offset, opt_uint arg_reg) {
    auto &tmp     = add_ir(IROp::StoreParam, bp_offset, arg_reg);
    tmp.data_size = size;
}

void GenIR::store(unsigned size, opt_uint to, opt_uint from) {
    auto &tmp     = add_ir(IROp::Store, to, from);
    tmp.data_size = size;
}

void GenIR::kill(opt_uint reg) { add_ir(IROp::Kill, reg, nullopt); }

void GenIR::load(unsigned size, opt_uint to, opt_uint from) {
    auto &tmp     = add_ir(IROp::Load, to, from);
    tmp.data_size = size;
}

IR &GenIR::add_ir(IROp op, opt_uint lhs, opt_uint rhs) {
    codes.emplace_back(op, lhs, rhs);
    return codes.back();
}

unsigned GenIR::new_reg() { return num_regs++; }

unsigned GenIR::new_label() { return num_labels++; }

opt_uint GenIR::gen_binary_op(IROp op, Token &lhs, Token &rhs) {
    auto left  = gen_expr(lhs);
    auto right = gen_expr(rhs);
    add_ir(op, left, right);
    kill(right);
    return left;
}

opt_uint GenIR::gen_left_value(Token &node) {
    auto kind = node.get_kind();
    if (kind == "MemberExpr") {
        auto reg    = gen_left_value(node.get_child(0));
        auto offset = node.get_offset();
        add_ir(IROp::AddImm, reg, offset);
        return reg;

    } else if (kind == "Identifier" && node.get_is_global()) {
        // Global Variable
        auto  reg           = new_reg();
        auto &tmp           = add_ir(IROp::LabelAddr, reg, nullopt);
        tmp.label_addr_name = node.get_value();
        return reg;

    } else if (kind == "Identifier" && !node.get_is_global()) {
        // Local Variable
        auto reg = new_reg();
        add_ir(IROp::BpOffset, reg, node.get_offset());
        return reg;

    } else if (kind == "IndexExpr") {
        /*
         *     数组下标操作
         *     1. 获得变量的offset
         *     2. 一层一层展开
         *     3. 上一次偏移+这一层位置*这一层宽度
         */
        auto &children = node.get_children();
        auto &ident    = children[0];
        auto  base_reg = gen_expr(ident);

        Type *current = ident.get_type();

        for (auto iter = children.begin() + 1; iter != children.end(); ++iter) {
            cout << "Pos: " << iter->get_value() << " Size: " << current->size << endl;
            if (current->c_type == Ctype::Array)
                current = current->base_type;
            else if (current->c_type == Ctype::Pointer)
                current = current->point_to;
            else {
                throw runtime_error("Fatal error: " + current->to_string());
            }
            // 获得当前层的目标位置
            auto current_pos  = gen_expr(*iter);
            auto current_size = new_reg();
            // 获得这一层的宽度，为立即数
            add_ir(IROp::Imm, current_size, current->size);
            // 相乘得到偏移
            add_ir(IROp::Mul, current_pos, current_size);
            // 加回目标寄存器
            add_ir(IROp::Add, base_reg, current_pos);
            kill(current_pos);
            kill(current_size);
            // add to base reg
        }
        return base_reg;
    }

    return nullopt;
}
void GenIR::label(opt_uint no) { add_ir(IROp::Label, no, nullopt); }

void GenIR::jmp(opt_uint no) { add_ir(IROp::Jmp, no, nullopt); }

void GenIR::comment(string text) {
    auto &tmp = add_ir(IROp::Comment, nullopt, nullopt);
    tmp.text  = move(text);
}

IROp GenIR::convert_compound_op(Token &op) {
    auto opc = op.get_value();
    cout << ": " << opc << endl;
    IROp res;
    if (opc == "+=") {
        res = IROp::Add;
    } else if (opc == "-=") {
        res = IROp::Sub;
    } else if (opc == "*=") {
        res = IROp::Mul;
    } else if (opc == "/=") {
        res = IROp::Div;
    } else if (opc == "%=") {
        res = IROp::Mod;
    } else if (opc == "&=") {
        res = IROp::And;
    } else if (opc == "|=") {
        res = IROp::Or;
    } else if (opc == "^=") {
        res = IROp::Xor;
    } else if (opc == "<<=") {
        res = IROp::Shl;
    } else if (opc == ">>=") {
        res = IROp::Shr;
    }
    return res;
}

opt_uint GenIR::gen_comp_assign(IROp op, unsigned size, Token &lhs, Token &rhs) {
    auto from = gen_expr(rhs);
    auto to   = gen_left_value(lhs);
    auto res  = new_reg();

    load(size, res, to);
    add_ir(op, res, from);
    kill(from);
    store(size, to, res);
    kill(to);
    return res;
}
