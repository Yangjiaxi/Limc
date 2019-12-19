#ifndef IR_4_IR_H_
#define IR_4_IR_H_

#include "0-token.h"

namespace Limc {

class Driver;

enum IROp {
    Add,      // +
    AddImm,   // +imm
    Sub,      // -
    SubImm,   // -imm
    Mul,      // *
    MulImm,   // *imm[Unused]
    Div,      // /
    Imm,      // 立即数
    BpOffset, // 加载相对于基址的偏移，是加载变量的第一步
    //典型使用： lea R[x], [rbp-offset]，把由[rbp-offset]指出的EA装载到R[x]中
    Mov,        // 目的是寄存器的移动，源操作数可以是立即数或寄存器
    Return,     // 从被调函数返回主调函数
    Call,       // 函数调用，含有至多六个参数寄存器[1,4,8bit]
    Label,      // 标号语句，用来放置
    LabelAddr,  // 全局变量的标号，对全局变量的取值通过标号给出
    EQ,         // ==
    NE,         // !=
    LE,         // <=
    LT,         // <
    GE,         // >=
    GT,         // >
    And,        // &&
    Or,         // ||
    Xor,        // ^
    Shl,        // <<
    Shr,        // >>
    Mod,        // %
    Neg,        // -x
    Jmp,        // 无条件跳转，目的由标号给出
    If,         // 测试真置条件位
    Unless,     //测试假置条件位
    Load,       // 根据算出的偏移加载某长度的数据，是加载变量的第二步
    Store,      // 赋值语句，把一个寄存器的值送入一个内存（栈）空间
    StoreParam, // 保存参数
    Kill,       // 寄存器丧失活性，被杀死，用于寄存器分配，不对应代码生成
    Nop,        // 无操作，用于寄存器分配
    Comment     // 为了IR方便看，没有实际作用
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
