#include "4-gen_ir.h"

#include "0-driver.h"
#include "util.h"

#define DEBUG

using namespace Limc;
using namespace std;

GenIR::GenIR(Driver &driver) : driver(driver), funcs(), codes() {}

vector<BasicFunc> GenIR::gen_ir(Token &token) {
    // TODO
    for (auto &block : token.get_children()) {

        auto kind = block.get_kind();

        if (kind == "FuncDef") {
            codes.clear();

            auto  name   = block.get_child(1).get_value();
            auto  stk_sz = block.get_func_stack_size();
            auto &params = block.get_child(2).get_children();
            auto &body   = block.get_child(3);

            for (unsigned long i = 0; i < params.size(); ++i) {
                auto &node = params[i];
                cout << i << " : " << offset << endl;
                store_param(node.get_type(), node.get_offset(), i);
            }

            cout << name << " " << stk_sz << endl;

            // auto &
        } else {
#ifdef DEBUG
            cout << "Pass `" << kind << "`.";
#endif
        }
    }
    return move(funcs);
}
