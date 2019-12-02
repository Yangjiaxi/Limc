#include "0-token.h"
#include "../lib/patterns.hpp"
#include "0-color.h"
#include "location.hh"
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

string Token::get_value() {
    if (name == "Pointer") {
        return get_child(0).get_value() + "*";
    } else {
        return value;
    }
}

vector<Token> &Token::get_children() { return children; }

Token &Token::get_child(int index) { return children.at(index); }

optional<location> Token::get_loc() const {
    // 最左 + 最右
    if (loc != nullopt) {
        return loc;
    }
    auto left  = children.cbegin();
    auto right = children.crbegin();
    while (left != children.cend() && right != children.crend()) {
        if (left->get_loc() != nullopt && right->get_loc() != nullopt) {
            location res;
            res.begin = left->get_loc().value().begin;
            res.end   = right->get_loc().value().end;
            return res;
        }
        if (left->get_loc() == nullopt)
            left++;
        if (right->get_loc() == nullopt)
            right++;
    }
    return nullopt;

    // if (loc == nullopt) {
    //     for (const auto &child : children) {
    //         auto child_loc = child.get_loc();
    //         if (child_loc != nullopt) {
    //             return child_loc;
    //         }
    //     }
    // }
    // return loc;
}

string Token::print(const string pre, const string ch_pre) const {
    stringstream s;
    s << endl;
    build_str(s, pre, GREEN);

    build_str(s, "{");

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
    build_str(s, "}");

    if (!type.empty()) {
        build_str(s, " [");
        build_str(s, type, BOLD_CYAN);
        build_str(s, "]");
    }

    if (loc != nullopt) {
        auto &[begin, end] = *loc;
        build_str(s, " (");
        s << RED << begin.line << "," << begin.column << RESET_COLOR;
        build_str(s, ")->(");
        s << RED << end.line << "," << end.column << RESET_COLOR;
        build_str(s, ")");
    }

    if (!children.empty()) {
        for (auto &item : children) {
            if (&item == &children.back()) {
                s << item.print(ch_pre + "└── ", ch_pre + "    ");
                // s << item.print(ch_pre + "`---", ch_pre + "    ");
            } else {
                s << item.print(ch_pre + "├── ", ch_pre + "│   ");
                // s << item.print(ch_pre + "|---", ch_pre + "|   ");
            }
        }
    }
    return s.str();
}

bool Token::operator==(const string &str) const { return name == str; }

void Token::set_type(const string &new_type) { type = new_type; }

// string &Token::get_type() const { return type; }
