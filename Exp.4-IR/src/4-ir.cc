#include "4-ir.h"

#include "0-driver.h"
#include "util.h"

using namespace Limc;
using namespace std;

IR::IR(IROp op, optional<unsigned> lhs, optional<unsigned> rhs) : op(op), lhs(lhs), rhs(rhs) {}

BasicFunc::BasicFunc(string name, vector<IR> ir, unsigned stack_size)
    : func_name(name), ir_list(move(ir)), stack_size(stack_size) {}
