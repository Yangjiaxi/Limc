#include "token.h"
#include "color.h"
#include <sstream>

using namespace Limc;

Token::Token(string name, string value)
    : name(move(name)), value(move(value)), children() {}

Token::Token(string name) : name(move(name)), value(), children() {}

Token::Token() : name(), value(), children() {}

Token::~Token() = default;

void Token::build_AST(const Token &child) { children.push_back(child); }

string Token::get_name() const { return name; }

string Token::get_value() const { return value; }

string Token::pretty_print(const string prefix,
                           const string child_prefix) const {
    stringstream s;
    s << endl;
    s << BOLD_GREEN << prefix << RESET_COLOR;
    s << "(" << (name == "ErrorStmt" ? BOLD_RED : BLUE) << name << RESET_COLOR;
    if (!value.empty()) {
        s << ", ";
        if (name == "Type") {
            s << MAGENTA;
        } else if (name == "Identifier") {
            s << CYAN;
        } else if (name == "Operator") {
            s << BOLD_YELLOW;
        } else {
            s << BOLD_GREEN;
        }
        s << value << RESET_COLOR;
    }
    s << ")";
    if (!children.empty()) {
        for (auto it = children.cbegin(); it != children.cend(); ++it) {
            if (it + 1 == children.cend()) {
                s << it->pretty_print(child_prefix + "└── ",
                                      child_prefix + "    ");
            } else {
                s << it->pretty_print(child_prefix + "├── ",
                                      child_prefix + "│   ");
            }
        }
    }
    return s.str();
}
