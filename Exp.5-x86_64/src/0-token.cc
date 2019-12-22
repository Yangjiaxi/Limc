#include "0-token.h"
#include "0-color.h"
#include "location.hh"
#include "util.h"
#include <optional>

using namespace Limc;

Token::Token(string kind, string value)
    : kind(move(kind)), value(move(value)), children(), loc(nullopt), func_stack_size(nullopt) {}

Token::Token(string kind)
    : kind(move(kind)), value(), children(), loc(nullopt), func_stack_size(nullopt) {}

Token::Token(string kind, string value, location loc)
    : kind(move(kind)), value(move(value)), children(), loc(loc), func_stack_size(nullopt) {}

Token::Token(string kind, location loc)
    : kind(move(kind)), value(), children(), loc(loc), func_stack_size(nullopt) {}

Token::Token() = default;

Token &Token::build_AST(Token child) {
    children.push_back(move(child));
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
    auto len   = children.size() - 1;
    auto left  = 0;
    auto right = len;
    while (left <= right) {
        auto left_loc  = children.at(left).get_loc();
        auto right_loc = children.at(right).get_loc();
        if (left_loc != nullopt && right_loc != nullopt) {
            location res;
            res.begin = left_loc.value().begin;
            res.end   = right_loc.value().end;
            return res;
        }
        if (left_loc == nullopt)
            ++left;
        if (right_loc == nullopt)
            --right;
    }
    return nullopt;
}

string Token::print(const string pre, const string ch_pre) const {
    stringstream s;
    s << endl;
    build_str(s, pre, BOLD_WHITE);

    build_str(s, "{");

    if (kind == "ErrorStmt") {
        build_str(s, kind, BOLD_RED);
    } else {
        build_str(s, kind, BLUE);
    }

    if (!value.empty()) {
        s << ", ";
        if (kind == "Type") {
            build_str(s, value, MAGENTA);
        } else if (kind == "Identifier") {
            build_str(s, value, CYAN);
        } else if (kind == "Operator") {
            build_str(s, value, BOLD_YELLOW);
        } else {
            build_str(s, value, BOLD_GREEN);
        }
    }

    build_str(s, "}");

    if (func_stack_size != nullopt) {
        build_str(s, " [");
        build_str(s, "Size: " + to_string(*func_stack_size), BOLD_YELLOW);
        build_str(s, "]");
    }

    if (is_global != nullopt && is_global.value()) {
        build_str(s, "[");
        build_str(s, "Global", BOLD_YELLOW);
        build_str(s, "]");
    }

    if (offset != nullopt) {
        build_str(s, "[");
        build_str(s, "Offset: " + to_string(*offset), BOLD_YELLOW);
        build_str(s, "]");
    }

    if (type != nullptr) {
        build_str(s, " [");
        build_str(s, type->to_string(), BOLD_YELLOW);
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
                // s << item.print(ch_pre + "└──", ch_pre + "     ");
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

void Token::set_type(Type *new_type) { type = new_type; }

Type *&Token::get_type() { return type; }

void Token::set_func_stack_size(unsigned n) { func_stack_size = n; }

void Token::set_offset(unsigned n) { offset = n; }

void Token::set_is_global(bool is_glb) { is_global = is_glb; }

unsigned Token::get_offset() { return offset.value(); }

unsigned Token::get_func_stack_size() { return func_stack_size.value(); }

void Token::replace_childs(vector<Token> new_nodes) {
    children.clear();
    children.insert(
        children.end(), make_move_iterator(new_nodes.begin()), make_move_iterator(new_nodes.end()));
}

bool Token::get_is_global() { return is_global.value(); }
