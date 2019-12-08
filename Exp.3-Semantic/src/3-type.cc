#include "3-type.h"
#include "util.h"

using namespace Limc;
using namespace std;

Type::Type() : base_type(nullptr), return_type(nullptr), members(), args() {}

Type *Type::build_literal(Token &root) {
    Type *new_type   = new Type();
    new_type->c_type = Ctype ::Plain;

    auto node_kind = root.get_kind();

    if (node_kind == "IntegerLiteral") {
        new_type->plain_type = PlainType::Int;
    } else if (node_kind == "FloatLiteral") {
        new_type->plain_type = PlainType::Float;
    } else if (node_kind == "CharLiteral") {
        new_type->plain_type = PlainType::Char;
    } else if (node_kind == "StringLiteral") {
        throw runtime_error("Not finish:  `StringLiteral`.");
    }
    return new_type;
}

Type *Type::build_type(Token &root, const vector<unsigned> &array_depth) {
    Type *new_type = new Type();
    Type *current  = new_type;
    if (!array_depth.empty()) {
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
        auto type_name  = root.get_value();
        if (type_name == "int") {
            current->plain_type = PlainType::Int;
            current->align = current->size = 4;
        } else if (type_name == "float") {
            current->plain_type = PlainType::Float;
            current->align = current->size = 4;
        } else if (type_name == "char") {
            current->plain_type = PlainType::Char;
            current->align = current->size = 1;
        } else if (type_name == "void") {
            current->plain_type = PlainType::Void;
            current->align = current->size = 0;
        }

    } else if (node_kind == "Struct") {
        // 结构体
        current->c_type  = Ctype::Struct;
        auto &   items   = root.get_child(0).get_children();
        unsigned s_size  = 0;
        unsigned s_align = 0;
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
                    auto             name        = ident_node.get_child(0).get_value();
                    vector<unsigned> depths      = make_array_depths(ident_node);
                    auto             member_type = build_type(item.get_child(0), depths);

                    s_size                     = align_memory(s_size, member_type->align);
                    member_type->member_offset = s_size;
                    s_size += member_type->size;
                    if (s_align < member_type->align) {
                        s_align = member_type->align;
                    }

                    current->members.insert({name, member_type});
                } else if (ident_node.get_kind() == "Identifier") {
                    // 成员为普通标识符
                    auto name        = ident_node.get_value();
                    auto member_type = build_type(item.get_child(0));

                    s_size                     = align_memory(s_size, member_type->align);
                    member_type->member_offset = s_size;
                    s_size += member_type->size;
                    if (s_align < member_type->align) {
                        s_align = member_type->align;
                    }

                    current->members.insert({name, member_type});
                }
            }
            current->align = s_align;
            current->size  = s_size;
        }
    } else if (node_kind == "FuncDecl" || node_kind == "FuncDef") {
        auto &ret            = root.get_child(0);
        auto &arguments      = root.get_child(2).get_children();
        current->c_type      = Ctype::Function;
        current->return_type = build_type(ret);
        // 对于函数来说，对齐和偏移量没啥用
        /// TODO 待证实
        current->align = current->size = 0;
        for (auto &arg : arguments) {
            assert(arg.get_kind() == "ParamDecl");
            current->args.push_back(build_type(arg.get_child(0)));
        }
    }
    make_array_info(new_type);
    return move(new_type);
}

// (size, align)
pair<unsigned, unsigned> Type::make_array_info(Type *root) {
    if (root->c_type == Ctype::Array) {
        auto [base_size, base_align] = make_array_info(root->base_type);
        root->size                   = base_size * root->length;
        root->align                  = base_align;
        return make_pair(root->size, root->align);
    } else {
        return make_pair(root->size, root->align);
    }
}

string Type::to_string() const {
    stringstream ss;
    if (c_type == Ctype::ErrorType) {
        ss << BOLD_RED << "ERROR" << RESET_COLOR;
        return ss.str();
    }

    ss << BOLD_GREEN;
    if (c_type == Ctype::Plain) {
        ss << "";
    } else if (c_type == Ctype::Struct) {
        ss << "{";
    } else if (c_type == Ctype::Array) {
        ss << "[";
    } else if (c_type == Ctype::Function) {
        ss << "Fn(";
    }

    ss << str_content();

    if (c_type == Ctype::Plain) {
        ss << "";
    } else if (c_type == Ctype::Struct) {
        ss << "}";
    } else if (c_type == Ctype::Array) {
        ss << "]";
    } else if (c_type == Ctype::Function) {
        ss << ")"
           << "->" << return_type->to_string();
    }
    if (c_type != Ctype::Function) {
        ss << "(" << size << "," << align << ")";
    }

    ss << RESET_COLOR;
    return ss.str();
}

string Type::str_content() const {
    stringstream ss;
    if (c_type == Ctype::Plain) {
        if (plain_type == PlainType::Int) {
            ss << "int";
        } else if (plain_type == PlainType::Float) {
            ss << "float";
        } else if (plain_type == PlainType::Char) {
            ss << "char";
        } else if (plain_type == PlainType::Void) {
            ss << "void";
        }
    } else if (c_type == Ctype::Array) {
        ss << length << ";" << base_type->to_string();
    } else if (c_type == Ctype::Struct) {
        bool first = true;
        for (auto &[name, type] : members) {
            ss << (first ? "" : " | ") << "<" << type->member_offset << ">" << name << ":"
               << type->to_string();
            first = false;
        }
    } else if (c_type == Ctype::Function) {
        bool first = true;
        for (auto &arg : args) {
            ss << (first ? "" : ",") << arg->to_string();
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

Type *Type::wrap_array(Type *root, const vector<unsigned> &array_depth) {
    Type *new_type = new Type();
    Type *current  = new_type;
    if (!array_depth.empty()) {
        for (auto iter = array_depth.cbegin(); iter != array_depth.cend() - 1; ++iter) {
            current->c_type    = Ctype::Array;
            current->length    = *iter;
            current->base_type = new Type();
            current            = current->base_type;
        }
        current->c_type    = Ctype::Array;
        current->length    = array_depth.back();
        current->base_type = root;
    }
    make_array_info(new_type);
    return new_type;
}
Type *Type::make_void_type() {
    Type *current       = new Type();
    current->c_type     = Ctype::Plain;
    current->plain_type = PlainType ::Void;
    current->align      = 0;
    current->size       = 0;
    return current;
}
Type *Type::make_error_type() {
    Type *current   = new Type();
    current->c_type = Ctype::ErrorType;
    return current;
}
