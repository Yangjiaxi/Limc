#include "token.h"
#include "../lib/patterns.hpp"
#include "color.h"
#include <optional>

using namespace Limc;
using mpark::patterns::_;
using mpark::patterns::arg;
using mpark::patterns::if_let;
using mpark::patterns::match;
using mpark::patterns::pattern;
using mpark::patterns::some;

static stringstream &build_str(stringstream &ss, const string &value, const char *color) {
    ss << color << value << RESET_COLOR;
    return ss;
}

static stringstream &build_str(stringstream &ss, const string &value) {
    ss << value;
    return ss;
}

Token::Token(string name, string value)
    : name(move(name)), value(move(value)), children(), loc(nullopt) {}

Token::Token(string name) : name(move(name)), value(), children(), loc(nullopt) {}

Token::Token(string name, string value, location loc)
    : name(move(name)), value(move(value)), children(), loc(loc) {}

Token::Token(string name, location loc) : name(move(name)), value(), children(), loc(loc) {}

Token::Token() = default;

Token &Token::build_AST(const Token &child) {
    children.push_back(child);
    return *this;
}

string Token::get_name() const { return name; }

void Token::set_name(const string &_name) { name = _name; }

string Token::get_value() const { return value; }

optional<location> Token::get_loc() const {
    if (loc == nullopt) {
        for (const auto &child : children) {
            auto child_loc = child.get_loc();
            if (child_loc != nullopt) {
                return child_loc;
            }
        }
    }
    return loc;
}

string Token::print(const string pre, const string ch_pre) const {
    stringstream s;
    s << endl;
    build_str(s, pre, GREEN);

    build_str(s, "(");

    match(name)(
        pattern("ErrorStmt") = [&] { build_str(s, name, BOLD_RED); },
        pattern(_)           = [&] { build_str(s, name, BLUE); });

    if (!value.empty()) {
        s << ", ";
        match(name)(
            pattern("Type")       = [&] { build_str(s, value, MAGENTA); },
            pattern("Identifier") = [&] { build_str(s, value, CYAN); },
            pattern("Operator")   = [&] { build_str(s, value, BOLD_YELLOW); },
            pattern(_)            = [&] { build_str(s, value, BOLD_GREEN); });
    }
    build_str(s, ")");

    if (loc != nullopt) {
        auto &[begin, end] = *loc;
        build_str(s, " @ (");
        s << RED << begin.line << "," << begin.column << RESET_COLOR;
        build_str(s, ")->(");
        s << RED << end.line << "," << end.column << RESET_COLOR;
        build_str(s, ")");
    }
    //  else {
    //     if_let(pattern(some(arg)) = get_loc()) = [&](location _loc) {
    //         auto &[begin, end] = _loc;
    //         build_str(s, " @ MK (");
    //         s << RED << begin.line << "," << begin.column << RESET_COLOR;
    //         build_str(s, ")->(");
    //         s << RED << end.line << "," << end.column << RESET_COLOR;
    //         build_str(s, ")");
    //     };
    // }

    if (!children.empty()) {
        for (auto &item : children) {
            if (&item == &children.back()) {
                s << item.print(ch_pre + "└── ", ch_pre + "    ");
            } else {
                s << item.print(ch_pre + "├── ", ch_pre + "│   ");
            }
        }
    }
    return s.str();
}
