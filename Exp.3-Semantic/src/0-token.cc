#include "0-token.h"
#include "0-color.h"
#include "location.hh"
#include "patterns.hpp"
#include "util.h"
#include <optional>

using namespace Limc;
using namespace mpark::patterns;

Token::Token(string kind, string value)
    : kind(move(kind)), value(move(value)), children(), loc(nullopt) {}

Token::Token(string kind) : kind(move(kind)), value(), children(), loc(nullopt) {}

Token::Token(string kind, string value, location loc)
    : kind(move(kind)), value(move(value)), children(), loc(loc) {}

Token::Token(string kind, location loc) : kind(move(kind)), value(), children(), loc(loc) {}

Token::Token() = default;

Token &Token::build_AST(const Token &child) {
    children.push_back(child);
    return *this;
}

string Token::get_kind() const { return kind; }

void Token::set_kind(const string &_kind) { kind = _kind; }

string Token::get_value() const { return value; }

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
}

string Token::print(const string pre, const string ch_pre) const {
    stringstream s;
    s << endl;
    build_str(s, pre, GREEN);

    build_str(s, "{");

    match(kind)(
        pattern("ErrorStmt") = [&] { build_str(s, kind, BOLD_RED); },
        pattern(_)           = [&] { build_str(s, kind, BLUE); });

    if (!value.empty()) {
        s << ", ";
        match(kind)(
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
                // s << item.print(ch_pre + "└──", ch_pre + "    ");
                s << item.print(ch_pre + "`---", ch_pre + "    ");
            } else {
                // s << item.print(ch_pre + "├──", ch_pre + "│   ");
                s << item.print(ch_pre + "|---", ch_pre + "|   ");
            }
        }
    }
    return s.str();
}

bool Token::operator==(const string &str) const { return kind == str; }

void Token::set_type(const string &new_type) { type = new_type; }

string &Token::get_type() { return type; }
