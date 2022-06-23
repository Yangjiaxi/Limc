#ifndef IR_4_IR_H_
#define IR_4_IR_H_

#include "0-token.h"

namespace Limc {

class Driver;

enum IROp {
    Add, // 四则运算 <Reg, Reg>
    Sub,
    Mul,
    Div,
    AddImm,   // 立即数运算 <Reg, Imm>
    SubImm,   //
    Imm,      // <Imm>
    BpOffset, // 加载相对于基址的偏移，是加载变量的第一步 <Reg, Imm>
    Mov, // 目的是寄存器的移动，源操作数可以是立即数或寄存器 <Reg, Reg|Imm>
    Return,    // 从被调函数返回主调函数 <Reg>
    Call,      // 函数调用，含有至多六个参数寄存器[1,4,8bit] <Reg> <name> <Reg...>
    Label,     // 标号语句，用来放置 <String>
    LabelAddr, // 全局变量的标号，对全局变量的取值通过标号给出 <Reg, String>
    EQ,        // == <Reg, Reg>
    NE,        // !=
    LE,        // <=
    LT,        // <
    GE,        // >=
    GT,        // >
    And,       // &&
    Or,        // ||
    Xor,       // ^
    Shl,       // <<
    Shr,       // >>
    Mod,       // %
    Neg,       // -x <Reg>
    Jmp,       // 无条件跳转，目的由标号给出 <String>
    If,        // 测试真置条件位 <Reg, String>
    Unless,    //测试假置条件位 <Reg, String>
    Load,  // 根据算出的偏移加载某长度的数据，是加载变量的第二步 <Reg, Mem>
    Store, // 赋值语句，把一个寄存器的值送入一个内存（栈）空间 <Mem, Reg>
    StoreParam, // 保存参数 <Mem, Reg>
    Kill, // 寄存器丧失活性，被杀死，用于寄存器分配，不对应代码生成 <Reg>
    Nop,  // 无操作，用于寄存器分配 <>
    Comment // 为了IR方便看，没有实际作用 <>
};

class IR {
  public:
    IR(IROp op, optional<unsigned> lhs, optional<unsigned> rhs);

    string to_string();

    // Call
    optional<string>           text           = nullopt;
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
    TyComment,
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
