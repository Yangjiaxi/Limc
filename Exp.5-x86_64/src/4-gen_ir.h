#ifndef IR_4_GEN_IR_H_
#define IR_4_GEN_IR_H_

#include "0-token.h"
#include "4-ir.h"

namespace Limc {

class Driver;

class GenIR {
    using opt_uint = optional<unsigned>;

  public:
    explicit GenIR(Driver &driver);

    vector<BasicFunc> gen_ir(Token &token);

  private:
    Driver &driver;
    vector<IR> codes;
    vector<BasicFunc> funcs;

    unsigned num_regs = 0;
    unsigned num_labels = 1;

    vector<unsigned> break_labels;
    vector<unsigned> continue_labels;

    unsigned new_reg();
    unsigned new_label();

    static IROp convert_compound_op(Token &op);

    IR &add_ir(IROp op, opt_uint lhs, opt_uint rhs);

    void gen_stmt(Token &node);
    opt_uint gen_expr(Token &node);

    opt_uint gen_left_value(Token &node);
    opt_uint gen_binary_op(IROp op, Token &lhs, Token &rhs);
    opt_uint gen_comp_assign(IROp op, unsigned size, Token &lhs, Token &rhs);
    opt_uint gen_pre_inc(unsigned size, Token &node, int num);
    opt_uint gen_post_inc(unsigned size, Token &node, int num);

    void comment(string text);
    void store_param(unsigned size, opt_uint bp_offset, opt_uint arg_reg);
    void store(unsigned size, opt_uint to, opt_uint from);
    void kill(opt_uint reg);
    void load(unsigned size, opt_uint to, opt_uint from);
    void label(opt_uint no);
    void jmp(opt_uint no);
};

} // namespace Limc

#endif // IR_4_GEN_IR_H_
