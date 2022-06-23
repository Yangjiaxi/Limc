#include "5-gen_x86-64.h"

#include "0-driver.h"
#include "util.h"

#define DEBUG

using namespace Limc;
using namespace std;

using opt_uint = optional<unsigned>;

GenX86_64::GenX86_64(Driver &driver) : driver(driver), asm_box() {}

vector<Code> GenX86_64::gen_x86_64(
    vector<BasicFunc> &funcs_ir, map<string, Symbol> &globals, map<string, string> &str_lits) {

    emit_header();
    emit_global(globals, str_lits);

    for (auto &func : funcs_ir) {
        emit_func(func);
    }

    // print();
    return move(asm_box);
}

Code &GenX86_64::emit() {
    asm_box.emplace_back();
    return asm_box.back();
}

void GenX86_64::emit_header() {
    emit().text(".intel_syntax noprefix").ok();
    emit().ok();
}

void GenX86_64::emit_global(map<string, Symbol> &globals, map<string, string> &str_lits) {
    emit().comment("Data Section").ok();
    emit().text(".data").ok();

    emit().comment("String Literals:").ok();
    // String Literals
    for (auto &[label, str] : str_lits) {
        emit().text(label).text(":").ok();
        emit().tab().text(".asciz").tab().text(str).ok();
    }
    emit().ok();

    emit().comment("Global Variables:").ok();
    for (auto &[name, info] : globals) {
        if (info.type->is_func())
            continue;
        emit().text(name).text(":").ok();
        emit()
            .tab()
            .text(".asciz")
            .tab()
            .text("\"")
            .text(repeat_str("\\000", info.type->size))
            .text("\"")
            .ok();
    }
    emit().ok();
}

void GenX86_64::print() {
    string dvd(50, '-');
    cout << dvd << endl;
    for (auto &code : asm_box) {
        cout << code.show();
    }
    cout << dvd << endl;
}

void GenX86_64::emit_func(BasicFunc &func) {
    emit().comment("Function Call: ").text(func.func_name).ok();

    end_label = ".Lend" + to_string(ret_label++); // 函数内部的return

    string func_name = "_" + func.func_name;

    emit().text(".text").ok();
    emit().text(".global").tab().text(func_name).ok();
    emit().text(func_name).text(":").ok();

    emit().comment("Active Record: ").ok();

    emit2("push", "rbp");
    emit3("mov", "rbp", "rsp");

    emit().comment("Stack Size: ").text(to_string(func.stack_size)).ok();
    emit3("sub", "rsp", to_string(align_memory(func.stack_size, 16)));
    emit2("push", "r12");
    emit2("push", "r13");
    emit2("push", "r14");
    emit2("push", "r15");

    emit().comment("end of prologue").ok();
    emit().ok();

    for (auto &ir : func.ir_list) {
        emit_code(ir);
    }
    emit().ok();

    emit().text(end_label).text(":").ok();
    emit().comment("end of procedure").ok();

    emit2("pop", "r15");
    emit2("pop", "r14");
    emit2("pop", "r13");
    emit2("pop", "r12");
    emit3("mov", "rsp", "rbp");
    emit2("pop", "rbp");
    emit().tab().text("ret").ok();

    emit().ok();
}

void GenX86_64::emit2(const string &code, const string &arg) {
    emit().tab().text(code).tab().text(arg).ok();
}
void GenX86_64::emit3(const string &code, const string &arg1, const string &arg2) {
    emit().tab().text(code).tab().text(arg1).comma().text(arg2).ok();
}

string to_int(opt_uint ref) { return to_string((int)ref.value()); }

void GenX86_64::emit_code(IR &ir) {
    // auto lhs = to_string(ir.lhs.value());
    // auto rhs = ir.rhs.value_or(0);
    auto &lhs = ir.lhs;
    auto &rhs = ir.rhs;
    auto &ref = ir.call_args_regs;
    switch (ir.op) {
        case IROp::StoreParam:
            emit3("mov", bp_minus(lhs), arg_reg(rhs, ir.data_size));
            break;
        case IROp::BpOffset:
            emit3("lea", reg(lhs), bp_minus(rhs));
            break;
        case IROp::Store:
            emit3("mov", mem(lhs), reg(rhs, ir.data_size));
            break;
        case IROp::Imm:
            emit3("mov", reg(lhs), to_int(rhs));
            break;
        case IROp::Load:
            emit3("mov", reg(lhs, ir.data_size), mem(reg(rhs)));
            if (ir.data_size == 1) {
                emit().tab().comment("Extend zero to high part.").ok();
                emit3("movzx", reg(lhs), reg(lhs, 1));
            }
            break;
        case IROp::LabelAddr:
            emit3("lea", reg(lhs), mem("rip+" + ir.label_addr_name.value()));
            break;
        case IROp::AddImm:
            emit3("add", reg(lhs), to_int(rhs));
            break;
        case IROp::SubImm:
            emit3("sub", reg(lhs), to_int(rhs));
            break;
        case IROp::Add:
            emit3("add", reg(lhs), reg(rhs));
            break;
        case IROp::Sub:
            emit3("sub", reg(lhs), reg(rhs));
            break;
        case IROp::Mul:
            emit3("mov", "rax", reg(rhs));
            emit2("mul", reg(lhs));
            emit3("mov", reg(lhs), "rax");
            break;
        case IROp::Div:
            emit3("mov", "rax", reg(lhs));
            emit().text("cqo").ok();
            emit2("div", reg(rhs));
            emit3("mov", reg(lhs), "rax");
            break;
        case IROp::Mod:
            emit3("mov", "rax", reg(lhs));
            emit().text("cqo").ok();
            emit2("div", reg(rhs));
            emit3("mov", reg(lhs), "rdx");
            break;
        case IROp::If:
            emit3("cmp", reg(lhs), "0");
            emit2("jne", ".L" + opt(rhs));
            break;
        case IROp::Unless:
            emit3("cmp", reg(lhs), "0");
            emit2("je", ".L" + opt(rhs));
            break;
        case IROp::Mov:
            emit3("mov", reg(lhs), reg(rhs));
            break;
        case IROp::Label:
            emit().text(".L" + opt(lhs) + ":").ok();
            break;
        case IROp::Jmp:
            emit2("jmp", ".L" + opt(lhs));
            break;
        case IROp::And:
            emit3("and", reg(lhs), reg(rhs));
            break;
        case IROp::Or:
            emit3("or", reg(lhs), reg(rhs));
            break;
        case IROp::Xor:
            emit3("xor", reg(lhs), reg(rhs));
            break;
        case IROp::Shl:
            emit3("mov", "cl", reg(rhs, 1));
            emit3("shl", reg(lhs), "cl");
            break;
        case IROp::Shr:
            emit3("mov", "cl", reg(rhs, 1));
            emit3("shr", reg(lhs), "cl");
            break;
        case IROp::GT:
            emit_relation(ir, "setg");
            break;
        case IROp::LT:
            emit_relation(ir, "setl");
            break;
        case IROp::GE:
            emit_relation(ir, "setge");
            break;
        case IROp::LE:
            emit_relation(ir, "setle");
            break;
        case IROp::EQ:
            emit_relation(ir, "sete");
            break;
        case IROp::NE:
            emit_relation(ir, "setne");
            break;
        case IROp::Neg:
            emit2("neg", reg(lhs));
            break;
        case IROp::Call:
            for (unsigned i = 0; i < ir.call_args_len; ++i) {
                emit3("mov", arg_reg(i), reg(ref.value()[i]));
            }
            emit2("push", "r10");
            emit2("push", "r11");
            emit3("mov", "rax", "0");
            emit().comment("Call function: ").text(ir.text.value()).ok();
            emit2("call", "_" + ir.text.value());
            emit2("pop", "r11");
            emit2("pop", "r10");
            emit3("mov", reg(lhs), "rax");
            break;
        case IROp::Return:
            emit3("mov", "rax", reg(lhs));
            emit2("jmp", end_label);
            break;
        case IROp::Nop:
        case IROp::Kill:
        case IROp::Comment:
            break;
        default:
            break;
    }
}

string GenX86_64::mem(opt_uint reg_no) { return mem(reg(reg_no)); }

string GenX86_64::mem(const string &addr) { return "[" + addr + "]"; }

string GenX86_64::bp_minus(opt_uint offset) { return mem("rbp-" + opt(offset)); }

string GenX86_64::arg_reg(opt_uint reg_no, opt_uint size) {
    auto val = size.value();
    auto idx = reg_no.value();

    if (val == 1) {
        return ARG_REG8[idx];
    } else if (val == 4) {
        return ARG_REG32[idx];
    } else if (val == 8) {
        return ARG_REG64[idx];
    } else {
        throw runtime_error("Fatal: size invalid: " + to_string(val));
    }
}

string GenX86_64::reg(opt_uint reg_no, opt_uint size) {
    auto val = size.value();
    auto idx = reg_no.value();

    if (val == 1) {
        return REG8[idx];
    } else if (val == 4) {
        return REG32[idx];
    } else if (val == 8) {
        return REG64[idx];
    } else {
        throw runtime_error("Fatal: size invalid: " + to_string(val));
    }
}

void GenX86_64::emit_relation(IR &ir, const string &test_cond) {
    auto &lhs = ir.lhs;
    auto &rhs = ir.rhs;
    emit3("cmp", reg(lhs), reg(rhs));
    emit2(test_cond, reg(lhs, 1));
    emit3("movzx", reg(lhs), reg(lhs, 1));
}
