#include "4-ir.h"
#include "0-driver.h"
#include "util.h"

using namespace Limc;
using namespace std;

#define WIDTH 15

using opt_uint = optional<unsigned>;

IR::IR(IROp op, opt_uint lhs, opt_uint rhs) : op(op), lhs(lhs), rhs(rhs) {}

template <typename T> string opt(optional<T> &ref) {
    if (ref == nullopt) {
        return "None";
    } else {
        return to_string(ref.value());
    }
}

void print_reg(stringstream &ss, opt_uint x, bool is_arg = false, bool no_pad = false) {
    build_str(
        ss, string(is_arg ? "Arg" : "") + "R[" + opt(x) + "]", BOLD_MAGENTA, no_pad ? 0 : WIDTH);
}

void print_code(stringstream &ss, const string &name, opt_uint size = nullopt) {
    if (size != nullopt) {
        build_str(ss, name, BOLD_GREEN, WIDTH - 1);
        build_str(ss, opt(size), BOLD_GREEN);
    } else {
        build_str(ss, name, BOLD_GREEN, WIDTH);
    }
}

void print_mem(stringstream &ss, opt_uint x) {
    build_str(ss, "M[R[" + opt(x) + "]]", BOLD_CYAN, WIDTH);
}

void print_imm(stringstream &ss, opt_uint x) {
    string str = "None";
    if (x != nullopt) {
        str = to_string((int)x.value());
    }
    build_str(ss, str, BOLD_YELLOW, WIDTH);
}

void print_label(stringstream &ss, opt_uint x, bool left = false) {
    build_str(ss, ".L" + opt(x), BOLD_RED, left ? 0 : WIDTH);
}

void print_label_addr(stringstream &ss, optional<string> &name) {
    build_str(ss, name.value_or("ERROR"), BOLD_RED, WIDTH);
}

string IR::to_string() {
    stringstream ss;
    auto         info = IRInfo::convert(op);

    /*
     *      寄存器： BOLD_MAGENTA
     *      立即数： BOLD_YELLOW
     *      内存：   BOLD_CYAN
     *      标签名： BOLD_BLUE
     */
    switch (info.ir_kind) {
        case IRType::TyComment:
            ss << "; " << text.value_or("");
            break;
        case IRType::TyStoreParam:
            print_code(ss, info.name, data_size);
            // print_mem(ss, lhs);
            ss << BOLD_CYAN << setw(WIDTH) << "M[BP-" + opt(lhs) + "]" << RESET_COLOR;
            ss << ",";
            print_reg(ss, rhs, true);
            break;
        case IRType::TyRegImm:
            print_code(ss, info.name);
            print_reg(ss, lhs);
            ss << ",";
            print_imm(ss, rhs);
            break;
        case IRType::TyMem:
            print_code(ss, info.name, data_size);
            if (op == IROp::Load) {
                print_reg(ss, lhs);
                ss << ",";
                print_mem(ss, rhs);
            } else {
                print_mem(ss, lhs);
                ss << ",";
                print_reg(ss, rhs);
            }
            break;
        case IRType::TyReg:
            print_code(ss, info.name);
            print_reg(ss, lhs);
            break;
        case IRType::TyLabelAddr:
            print_code(ss, info.name);
            print_reg(ss, lhs);
            ss << ",";
            print_label_addr(ss, label_addr_name);
            break;
        case IRType::TyRegReg:
            print_code(ss, info.name);
            print_reg(ss, lhs);
            ss << ",";
            print_reg(ss, rhs);
            break;
        case IRType::TyRegLabel:
            print_code(ss, info.name);
            print_reg(ss, lhs);
            ss << ",";
            print_label(ss, rhs);
            break;
        case IRType::TyLabel:
            print_label(ss, lhs, true);
            break;
        case IRType::TyJmp:
            print_code(ss, info.name);
            print_label(ss, lhs);
            break;
        case IRType::TyCall:
            print_code(ss, info.name);
            print_reg(ss, lhs);
            ss << ",\t" << BOLD_RED << text.value() << "(" << RESET_COLOR;
            for (unsigned i = 0; i < call_args_len; ++i) {
                if (i != 0) {
                    ss << ", ";
                }
                print_reg(ss, call_args_regs.value()[i], false, true);
            }
            ss << BOLD_RED << ")" << RESET_COLOR;
            break;
        default:
            break;
    }

    return ss.str();
}

BasicFunc::BasicFunc(string name, vector<IR> ir, unsigned stack_size)
    : func_name(name), ir_list(move(ir)), stack_size(stack_size) {}

void BasicFunc::print_ir() {
    cout << string(50, '-') << endl;
    cout << func_name << "(): " << stack_size << endl;
    for (auto &item : ir_list) {
        cout << item.to_string() << endl;
    }
}

IRInfo::IRInfo(string name, IRType ir_kind) : name(move(name)), ir_kind(ir_kind) {}

IRInfo IRInfo::convert(IROp op) {
    // cout << "IROp: " << op << endl;
    switch (op) {
        case IROp::StoreParam:
            return IRInfo("STORE_ARG", IRType::TyStoreParam);
        case IROp::BpOffset:
            return IRInfo("BPOFF", IRType::TyRegImm);
        case IROp::Store:
            return IRInfo("STORE", IRType::TyMem);
        case IROp::Kill:
            return IRInfo("KILL", IRType::TyReg);
        case IROp::Imm:
            return IRInfo("MOV", IRType::TyRegImm);
        case IROp::Load:
            return IRInfo("LOAD", IRType::TyMem);
        case IROp::LabelAddr:
            return IRInfo("LABEL_ADDR", IRType::TyLabelAddr);
        case IROp::AddImm:
            return IRInfo("ADD", IRType::TyRegImm);
        case IROp::SubImm:
            return IRInfo("SUB", IRType::TyRegImm);
        case IROp::Add:
            return IRInfo("ADD", IRType::TyRegReg);
        case IROp::Sub:
            return IRInfo("SUB", IRType::TyRegReg);
        case IROp::Mul:
            return IRInfo("MUL", IRType::TyRegReg);
        case IROp::Div:
            return IRInfo("DIV", IRType::TyRegReg);
        case IROp::Mod:
            return IRInfo("MOD", IRType::TyRegReg);
        case IROp::Unless:
            return IRInfo("IFNOT", IRType::TyRegLabel);
        case IROp::Mov:
            return IRInfo("MOV", IRType::TyRegReg);
        case IROp::Label:
            return IRInfo("", IRType::TyLabel);
        case IROp::Jmp:
            return IRInfo("JMP", IRType::TyJmp);
        case IROp::Comment:
            return IRInfo("", IRType::TyComment);
        case IROp::And:
            return IRInfo("AND", IRType::TyRegReg);
        case IROp::Or:
            return IRInfo("OR", IRType::TyRegReg);
        case IROp::Xor:
            return IRInfo("XOR", IRType::TyRegReg);
        case IROp::Shl:
            return IRInfo("SHL", IRType::TyRegReg);
        case IROp::Shr:
            return IRInfo("SHR", IRType::TyRegReg);
        case IROp::GT:
            return IRInfo("GT", IRType::TyRegReg);
        case IROp::LT:
            return IRInfo("LT", IRType::TyRegReg);
        case IROp::GE:
            return IRInfo("GE", IRType::TyRegReg);
        case IROp::LE:
            return IRInfo("LE", IRType::TyRegReg);
        case IROp::EQ:
            return IRInfo("EQ", IRType::TyRegReg);
        case IROp::NE:
            return IRInfo("NE", IRType::TyRegReg);
        case IROp::Neg:
            return IRInfo("NEG", IRType::TyReg);
        case IROp::Call:
            return IRInfo("CALL", IRType::TyCall);
        case IROp::Return:
            return IRInfo("RET", IRType::TyReg);
        default:
            break;
    }
    // unreachable
    return IRInfo();
}
