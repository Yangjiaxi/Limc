#include "3-type.h"
#include "patterns.hpp"

using namespace Limc;
using namespace std;
using namespace mpark::patterns;

Type::Type() : base_type(nullptr), return_type(nullptr), members() {}

Type Type::build_type(Token &root, vector<unsigned> array_depth) {
    Type  new_type = Type();
    Type *current  = &new_type;
    if (array_depth.size()) {
        for (auto &dep : array_depth) {
            current->c_type    = Ctype::Array;
            current->length    = dep;
            current->base_type = new Type();
            current            = current->base_type;
        }
    }
    if (root.get_kind() == "Type") {
        current->c_type = Ctype::Plain;
        match(root.get_value())(
            pattern("int")   = [&] { current->plain_type = PlainType::Int; },
            pattern("float") = [&] { current->plain_type = PlainType::Float; },
            pattern("char")  = [&] { current->plain_type = PlainType::Char; },
            pattern("void")  = [&] { current->plain_type = PlainType::Void; }

        );

    } else if (root.get_kind() == "Struct") {
        current->c_type = Ctype::Struct;
        auto &items     = root.get_child(0).get_children();
        for (auto &item : items) {
            // 标识符的节点，建立数组或是普通
            // 0 -> Struct 类型
            // 1 -> VarList
            auto ident_list = item.get_child(1).get_children();
            for (auto &ident_node_wrap : ident_list) {
                assert(ident_node_wrap.get_kind() == "VarDecl");
                auto &ident_node = ident_node_wrap.get_child(0);
                if (ident_node.get_kind() == "IndexExpr") {
                    auto             name   = ident_node.get_child(0).get_value();
                    vector<unsigned> depths = make_array_depths(ident_node);
                    current->members.insert({name, build_type(item.get_child(0), depths)});
                } else if (ident_node.get_kind() == "Identifier") {
                    auto name = ident_node.get_value();
                    current->members.insert({name, build_type(item.get_child(0))});
                }
            }
        }
    }
    return move(new_type);
}

string Type::to_string() const {
    stringstream ss;
    match(c_type)(
        pattern(Ctype::Plain)    = [&ss] { ss << ""; },
        pattern(Ctype::Struct)   = [&ss] { ss << "{"; },
        pattern(Ctype::Array)    = [&ss] { ss << "["; },
        pattern(Ctype::Function) = [&ss] { ss << "Fn("; }

    );
    ss << str_content();
    match(c_type)(
        pattern(Ctype::Plain)    = [&ss] { ss << ""; },
        pattern(Ctype::Struct)   = [&ss] { ss << "}"; },
        pattern(Ctype::Array)    = [&ss] { ss << "]"; },
        pattern(Ctype::Function) = [&ss] { ss << ")"; }

    );
    return ss.str();
}

string Type::str_content() const {
    stringstream ss;
    if (c_type == Ctype::Plain) {
        match(plain_type)(
            pattern(PlainType::Int)   = [&] { ss << "int"; },
            pattern(PlainType::Float) = [&] { ss << "float"; },
            pattern(PlainType::Char)  = [&] { ss << "char"; },
            pattern(PlainType::Void)  = [&] { ss << "void"; });
    } else if (c_type == Ctype::Array) {
        ss << length << ";" << base_type->to_string();
    } else if (c_type == Ctype::Struct) {
        bool first = true;
        for (auto &[name, type] : members) {
            ss << (first ? "" : " | ") << name << ":" << type.to_string();
            first = false;
        }
    }
    return ss.str();
}

vector<unsigned> Type::make_array_depths(Token &root) {
    vector<unsigned> res;
    auto &           children = root.get_children();
    for (auto iter = children.begin() + 1; iter != children.end(); ++iter) {
        auto &c0 = iter->get_child(0);
        assert(c0.get_kind() == "IntegerLiteral");
        res.push_back(stoi(c0.get_value()));
    }
    return res;
}
