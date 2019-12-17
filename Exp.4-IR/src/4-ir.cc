#include "4-ir.h"

#include "0-driver.h"
#include "util.h"

using namespace Limc;
using namespace std;

IRContainer::IRContainer() : container() {}

IR &IRContainer::append() {
    container.emplace_back();
    return container.back();
}

IR::IR() : op(), arg1(), arg2(), res() {}

IR &IR::set_op(string _op) {
    op = move(_op);
    return *this;
}

IR &IR::set_arg1(string _arg1) {
    arg1 = move(_arg1);
    return *this;
}

IR &IR::set_arg2(string _arg2) {
    arg2 = move(_arg2);
    return *this;
}

IR &IR::set_res(string _res) {
    res = move(_res);
    return *this;
}
string IR::to_string() const {
    stringstream ss;
    ss << "(";
    build_str(ss, op, BOLD_YELLOW, 10);
    ss << ", ";
    build_str(ss, arg1, BOLD_BLUE, 5);
    ss << ", ";
    build_str(ss, arg2, BOLD_BLUE, 5);
    ss << ", ";
    build_str(ss, res, BOLD_GREEN, 5);
    ss << ")";
    return ss.str();
}
