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
void IRContainer::show() {
    show_head();
    for (auto &item : container) {
        cout << item.to_string() << endl;
    }
}

void IRContainer::show_head() {
    stringstream ss;
    build_str(ss, "OP", BOLD_YELLOW, 11);
    build_str(ss, "ARG_1", BOLD_BLUE, 13);
    build_str(ss, "ARG_2", BOLD_BLUE, 12);
    build_str(ss, "RESULT", BOLD_GREEN, 11);
    cout << ss.str() << endl;
}
unsigned IRContainer::size() const { return container.size(); }
IR &IRContainer::operator[](unsigned i) { return container[i]; }

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

string padding(string s) {
    if (s.empty())
        return "____";
    else
        return move(s);
}

string IR::to_string() const {
    stringstream ss;
    ss << "(";
    build_str(ss, padding(op), YELLOW, 10);
    ss << ", ";
    build_str(ss, padding(arg1), BLUE, 10);
    ss << ", ";
    build_str(ss, padding(arg2), BLUE, 10);
    ss << ", ";
    build_str(ss, padding(res), GREEN, 10);
    ss << ")";
    return ss.str();
}
