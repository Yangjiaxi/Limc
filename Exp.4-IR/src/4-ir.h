#ifndef IR_4_IR_H_
#define IR_4_IR_H_

#include "0-token.h"

namespace Limc {

class Driver;

enum IROp {
    Add,
    AddImm,
    Sub,
    SubImm,
    Mul,
    MulImm,
    Div,
    Imm,
    SetBP,
    Mov,
    Return,
    Call,
    Label,
    LabelAddr,
    EQ,
    NE,
    LE,
    LT,
    AND,
    OR,
    XOR,
    SHL,
    SHR,
    Mod,
    Neg,
    Jmp,
    If,
    Unless,
    Load,
    Store,
    StoreArg,
    Kill,
    Nop
};

class IR {
  public:
    IR(IROp op, optional<unsigned> lhs, optional<unsigned> rhs);

  private:
    IROp               op;
    optional<unsigned> lhs;
    optional<unsigned> rhs;
};

class BasicFunc {
  public:
    BasicFunc(string name, vector<IR> ir, unsigned stack_size);

  private:
    string     func_name;
    vector<IR> ir_list;
    unsigned   stack_size;

    // Call
    optional<string>           call_name      = nullopt;
    optional<unsigned>         call_args_len  = nullopt;
    optional<vector<unsigned>> call_args_regs = nullopt;

    // LabelAddr
    optional<string> label_addr_name = nullopt;

    // Load, Store, StoreArg
    optional<unsigned> data_size = nullopt;
};

} // namespace Limc

#endif // IR_4_HIR_H_
