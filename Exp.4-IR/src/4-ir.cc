#include "4-ir.h"

#include "0-driver.h"
#include "util.h"

using namespace Limc;
using namespace std;

using opt_uint = optional<unsigned>;

IR::IR(IROp op, opt_uint lhs, opt_uint rhs) : op(op), lhs(lhs), rhs(rhs) {}

string opt(opt_uint &ref) {
    if (ref == nullopt) {
        return "None";
    } else {
        return to_string(ref.value());
    }
}

string print_reg(opt_uint x, bool is_arg = false) {
    stringstream ss;
    build_str(ss, string(is_arg ? "a" : "") + "R(" + opt(x) + ")", BOLD_MAGENTA, 8);
    return ss.str();
}

string print_mem(opt_uint x) {}

string print_imm(opt_uint x) {}

string print_label(opt_uint x) {}

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
        case IRType::TyStoreParam:
            // ss << "\t";
            build_str(ss, info.name, BOLD_GREEN, 14);
            build_str(ss, opt(data_size), BOLD_GREEN);
            build_str(ss, "M(" + opt(lhs) + ")", BOLD_CYAN, 8);
            ss << ",";
            ss << print_reg(rhs, true);
            // build_str(ss, "aR(" + opt(rhs) + ")", BOLD_MAGENTA, 8);
            break;
        case IRType::TyRegImm:
            // ss << "\t";
            build_str(ss, info.name, BOLD_GREEN, 15);
            ss << print_reg(lhs);
            // build_str(ss, "R(" + opt(lhs) + ")", BOLD_MAGENTA, 8);
            ss << ",";
            build_str(ss, opt(rhs), BOLD_YELLOW, 8);
            break;
        case IRType::TyMem:
            // ss << "\t";
            build_str(ss, info.name, BOLD_GREEN, 14);
            build_str(ss, opt(data_size), BOLD_GREEN);
            ss << print_reg(lhs);
            // build_str(ss, "R(" + opt(lhs) + ")", BOLD_MAGENTA, 8);
            ss << ",";
            build_str(ss, "M(" + opt(rhs) + ")", BOLD_CYAN, 8);
            break;
        case IRType::TyReg:
            // ss << "\t";
            build_str(ss, info.name, BOLD_GREEN, 15);
            ss << print_reg(lhs);
            // build_str(ss, "R(" + opt(lhs) + ")", BOLD_MAGENTA, 8);
            break;
        case IRType::TyLabelAddr:
            // ss << "\t";
            build_str(ss, info.name, BOLD_GREEN, 15);
            ss << print_reg(lhs);
            // build_str(ss, "R(" + opt(lhs) + ")", BOLD_MAGENTA, 8);
            ss << ",";
            build_str(ss, label_addr_name.value_or("ERROR"), BOLD_RED, 8);
            break;
        case IRType::TyRegReg:
            // ss << "\t";
            build_str(ss, info.name, BOLD_GREEN, 15);
            ss << print_reg(lhs);
            // build_str(ss, "R(" + opt(lhs) + ")", BOLD_MAGENTA, 8);
            ss << ",";
            ss << print_reg(rhs);
            // build_str(ss, "R(" + opt(rhs) + ")", BOLD_MAGENTA, 8);
            break;
        case IRType::TyRegLabel:
            // ss << "\t";
            build_str(ss, info.name, BOLD_GREEN, 15);
            ss << print_reg(lhs);
            // build_str(ss, "R(" + opt(lhs) + ")", BOLD_MAGENTA, 8);
            ss << ",";
            build_str(ss, ".L" + opt(rhs), BOLD_RED, 8);
            break;
        case IRType::TyLabel:
            build_str(ss, ".L" + opt(lhs), BOLD_RED);
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
            return IRInfo("PUSH", IRType::TyStoreParam);
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
        default:
            break;
    }
    // unreachable
    return IRInfo();
}
