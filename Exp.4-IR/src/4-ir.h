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
    BpOffset,
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
    StoreParam,
    Kill,
    Nop
};

class IR {
  public:
    IR(IROp op, optional<unsigned> lhs, optional<unsigned> rhs);

    string to_string();

    // Call
    optional<string>           call_name      = nullopt;
    optional<unsigned>         call_args_len  = nullopt;
    optional<vector<unsigned>> call_args_regs = nullopt;

    // LabelAddr
    optional<string> label_addr_name = nullopt;

    // Load, Store, StoreParam
    optional<unsigned> data_size = nullopt;

  private:
    IROp               op;
    optional<unsigned> lhs;
    optional<unsigned> rhs;
};

class BasicFunc {
  public:
    BasicFunc(string name, vector<IR> ir, unsigned stack_size);

    void print_ir();

  private:
    string     func_name;
    vector<IR> ir_list;
    unsigned   stack_size;
};

enum IRType {
    TyNoArg,
    TyReg,
    TyImm,
    TyMem,
    TyJmp,
    TyLabel,
    TyLabelAddr,
    TyRegReg,
    TyRegImm,
    TyStoreParam,
    TyRegLabel,
    TyCall,
};

class IRInfo {
  public:
    IRInfo() = default;
    IRInfo(string name, IRType ir_kind);
    string name;
    IRType ir_kind;

    static IRInfo convert(IROp op);
};

} // namespace Limc

#endif // IR_4_HIR_H_
