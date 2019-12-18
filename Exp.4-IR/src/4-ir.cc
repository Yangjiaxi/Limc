#include "4-ir.h"

#include "0-driver.h"
#include "util.h"

using namespace Limc;
using namespace std;

IR::IR(IROp op, optional<unsigned> lhs, optional<unsigned> rhs) : op(op), lhs(lhs), rhs(rhs) {}

string opt(optional<unsigned> &ref) {
    if (ref == nullopt) {
        return "None";
    } else {
        return to_string(ref.value());
    }
}

string IR::to_string() {
    stringstream ss;
    auto         info = IRInfo::convert(op);

    switch (info.ir_kind) {
        case IRType::TyStoreParam:
            ss << "\t" << info.name << opt(data_size) << "\t" << opt(lhs) << ",\t" << opt(rhs);
            break;
        case IRType::TyRegImm:
            ss << "\t" << info.name << "\tR(" << opt(lhs) << "),\t" << opt(rhs);
            break;
        case IRType::TyMem:
            ss << "\t" << info.name << opt(data_size) << "\tR(" << opt(lhs) << "),\t" << opt(rhs);
            break;
        case IRType::TyReg:
            ss << "\t" << info.name << "\tR(" << opt(lhs) << ")";
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
    cout << func_name << "():" << endl;
    for (auto &item : ir_list) {
        cout << item.to_string() << endl;
    }
}

IRInfo::IRInfo(string name, IRType ir_kind) : name(move(name)), ir_kind(ir_kind) {}

IRInfo IRInfo::convert(IROp op) {
    // cout << "IROp: " << op << endl;
    switch (op) {
        case IROp::StoreParam:
            return move(IRInfo("PUSH", IRType::TyStoreParam));
        case IROp::BpOffset:
            return move(IRInfo("BPOFF", IRType::TyRegImm));
        case IROp::Store:
            return move(IRInfo("STORE", IRType::TyMem));
        case IROp::Kill:
            return move(IRInfo("KILL", IRType::TyReg));
        default:
            break;
    }
    // unreachable
    return move(IRInfo());
}
