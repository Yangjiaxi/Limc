#include "4-gen_ir.h"

#include "0-driver.h"
#include "util.h"

#define DEBUG

using namespace Limc;
using namespace std;
using opt_uint = optional<unsigned>;

GenIR::GenIR(Driver &driver) : driver(driver), funcs(), codes() {}

optional<unsigned> GenIR::gen_expr(Token &node) { return 666; }

void GenIR::gen_stmt(Token &node) {
    auto  kind     = node.get_kind();
    auto &children = node.get_children();
#ifdef DEBUG
    cout << "Gen for `" << kind << "`." << endl;
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
    // Fall to
    else {
        cout << BOLD_RED << "NOT: " << kind << RESET_COLOR << endl;
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

IR &GenIR::add_ir(IROp op, opt_uint lhs, opt_uint rhs) {
    codes.emplace_back(op, lhs, rhs);
    return codes.back();
}

unsigned GenIR::new_reg() { return num_regs++; }
