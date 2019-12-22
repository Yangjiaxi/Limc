#include "5-reg_alloc.h"
#include "0-driver.h"
#include "util.h"

using namespace Limc;
using namespace std;
using opt_uint = optional<unsigned>;

RegAllocator::RegAllocator(Driver &driver)
    : driver(driver), prev_mapped_regs(1 << 10, nullopt), current_in_use(REG_NUM, false) {}

unsigned RegAllocator::map_reg(unsigned fake_reg) {
    if (prev_mapped_regs.at(fake_reg) != nullopt) {
        return prev_mapped_regs[fake_reg].value();
    }
    for (unsigned i = 0; i < REG_NUM; ++i) {
        if (current_in_use[i])
            continue;
        prev_mapped_regs[fake_reg] = i;
        current_in_use[i]          = true;
        return i;
    }
    throw runtime_error("Fatal: Register out of use!");
}

void RegAllocator::alloc_ir(vector<IR> &irs) {

    for (auto &ir : irs) {
        auto  info = IRInfo::convert(ir.op);
        auto &ref  = ir.call_args_regs;
        switch (info.ir_kind) {
            case IRType::TyRegImm:
            case IRType::TyReg:
            case IRType::TyRegLabel:
            case IRType::TyLabelAddr:
                ir.lhs = map_reg(ir.lhs.value());
                break;
            case IRType::TyMem:
            case IRType::TyRegReg:
                ir.lhs = map_reg(ir.lhs.value());
                ir.rhs = map_reg(ir.rhs.value());
                break;
            case IRType::TyCall:
                ir.lhs = map_reg(ir.lhs.value());
                for (unsigned i = 0; i < ir.call_args_len; ++i) {
                    ref.value()[i] = map_reg(ref.value()[i]);
                }
                break;
            default:
                break;
        }

        if (ir.op == IROp::Kill) {
            current_in_use[ir.lhs.value()] = false;
            ir.op                          = IROp::Nop;
        }
    }
}

void RegAllocator::reg_alloc(vector<BasicFunc> &funcs) {
    for (auto &func : funcs) {
        current_in_use = vector(7, false);
        alloc_ir(func.ir_list);
    }
}
