#include "3-type.h"
#include "patterns.hpp"

using namespace Limc;
using namespace std;
using namespace mpark::patterns;

Type::Type() : base_type(nullptr), return_type(nullptr), members(), args() {}

Type Type::build_type(Token &root, vector<unsigned> array_depth) {
    Type  new_type = Type();
    Type *current  = &new_type;
    if (array_depth.size()) {
        // 嵌套数组
        // array_depth保存了数组每一层的大小，最后一维是基类型的长度
        // int a[3][4] => build_type(`int`, Vec<usize>{3, 4});
        for (auto &dep : array_depth) {
            current->c_type    = Ctype::Array;
            current->length    = dep;
            current->base_type = new Type();
            current            = current->base_type;
        }
    }

    auto node_kind = root.get_kind();
    if (node_kind == "Type") {
        // 普通类型
        current->c_type = Ctype::Plain;
        match(root.get_value())(
            pattern("int")   = [&] { current->plain_type = PlainType::Int; },
            pattern("float") = [&] { current->plain_type = PlainType::Float; },
            pattern("char")  = [&] { current->plain_type = PlainType::Char; },
            pattern("void")  = [&] { current->plain_type = PlainType::Void; }

        );
    } else if (node_kind == "Struct") {
        // 结构体
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
                    // 成员定义为数组
                    auto             name   = ident_node.get_child(0).get_value();
                    vector<unsigned> depths = make_array_depths(ident_node);
                    current->members.insert({name, build_type(item.get_child(0), depths)});
                } else if (ident_node.get_kind() == "Identifier") {
                    // 成员为普通标识符
                    auto name = ident_node.get_value();
                    current->members.insert({name, build_type(item.get_child(0))});
                }
            }
        }
    } else if (node_kind == "FuncDecl" || node_kind == "FuncDef") {
        auto &ret            = root.get_child(0);
        auto &arguments      = root.get_child(2).get_children();
        current->c_type      = Ctype::Function;
        current->return_type = new Type();
        current->return_type->build_type(ret);
        for (auto &arg : arguments) {
            assert(arg.get_kind() == "ParamDecl");
            current->args.push_back(build_type(arg.get_child(0)));
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
    if (c_type == Ctype::Function) {
        ss << "->" << return_type->to_string();
    }
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
    } else if (c_type == Ctype::Function) {
        bool first = true;
        for (auto &arg : args) {
            ss << (first ? "" : ",") << arg.to_string();
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
