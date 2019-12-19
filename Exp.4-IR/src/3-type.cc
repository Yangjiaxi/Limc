#include "3-type.h"
#include "util.h"

// #define VERBOSE

using namespace Limc;
using namespace std;

Type::Type() : base_type(nullptr), return_type(nullptr), members(), args(), align(0), size(0) {}

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

Type *
Type::build_type(Token &root, const vector<unsigned> &array_depth, map<string, Type *> type_table) {
    Type *new_type = new Type();
    Type *current  = new_type;

    auto node_kind = root.get_kind();

    if (!array_depth.empty()) {
        // 嵌套数组
        // array_depth保存了数组每一层的大小，最后一维是基类型的长度
        // int a[3][4] => build_type(`int`, Vec<usize>{3, 4});
        for (auto iter = array_depth.cbegin(); iter != array_depth.cend(); ++iter) {
            current->c_type = Ctype::Array;
            current->length = *iter;

            if (iter == array_depth.cend() - 1) {
                if (node_kind == "Struct" && root.get_children().empty()) {
                    // 内部 结构体 无成员
                    if (type_table.find(root.get_value()) == type_table.end()) {
                        throw runtime_error("Unknown struct type `" + root.get_value() + "`");
                    }
                    current->base_type = type_table.at(root.get_value());
                    make_array_info(new_type);
                    return new_type;
                }
            }

            current->base_type = new Type();
            current            = current->base_type;
        }
    }

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
        current->c_type = Ctype::Struct;

        if (root.get_children().empty()) {
            // 内部 结构体 无成员
            if (type_table.find(root.get_value()) == type_table.end()) {
                throw runtime_error("Unknown struct type `" + root.get_value() + "`");
            }
            return type_table.at(root.get_value());
        }

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
                    auto             name   = ident_node.get_child(0).get_value();
                    vector<unsigned> depths = make_array_depths(ident_node);
                    auto member_type        = build_type(item.get_child(0), depths, type_table);

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
                    auto member_type = build_type(item.get_child(0), {}, type_table);

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
        current->return_type = build_type(ret, {}, type_table);
        // 对于函数来说，对齐和偏移量没啥用
        /// TODO 待证实
        current->align = current->size = 0;
        for (auto &arg : arguments) {
            assert(arg.get_kind() == "ParamDecl");
            // current->args.push_back(build_type(arg.get_child(0), {}, type_table));
            Type *inner_type = build_type(arg.get_child(0), {}, type_table);
            auto &ident_node = arg.get_child(1);
            if (ident_node.get_kind() == "Identifier") {
                // cout << "ident" << endl;
                current->args.push_back(inner_type);
            } else {
                // cout << "array" << endl;
                assert(ident_node.get_kind() == "IndexExpr");
                auto inner_depths = make_array_depths(ident_node);
                inner_type        = wrap_array(inner_type, inner_depths);
                // cout << "array: " << inner_type->to_string() << endl;
                // 把数组参数改成指针
                /*
                 * void fn(int a[10][10])
                 *  => void | fn | Ptr(_)->[10;int]
                 */
                inner_type->c_type    = Ctype::Pointer;
                inner_type->size      = 8;
                inner_type->align     = 8;
                inner_type->point_to  = inner_type->base_type;
                inner_type->base_type = nullptr;
                current->args.push_back(inner_type);
            }
        }
    }
    make_array_info(new_type);
    return new_type;
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
    ss << BOLD_GREEN << str_wrap() << RESET_COLOR;
    return ss.str();
}

string Type::str_wrap() const {
    stringstream ss;

    if (is_plain()) {
        ss << "";
    } else if (c_type == Ctype::Struct) {
        ss << "{";
    } else if (c_type == Ctype::Array) {
        ss << "[";
    } else if (c_type == Ctype::Pointer) {
        ss << "Ptr<";
    } else if (c_type == Ctype::Function) {
        ss << "Fn(";
    }

    ss << str_content();

    if (is_plain()) {
        ss << "";
    } else if (c_type == Ctype::Struct) {
        ss << "}";
    } else if (c_type == Ctype::Array) {
        ss << "]";
    } else if (c_type == Ctype::Pointer) {
        ss << ">";
    } else if (c_type == Ctype::Function) {
        ss << ")"
           << "->" << return_type->str_wrap();
    }

    if (c_type != Ctype::Function) {
#ifdef VERBOSE
        ss << "(" << size << "," << align << ")";
#endif
    }

    return ss.str();
}

string Type::str_content() const {
    stringstream ss;
    if (is_plain()) {
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
        ss << length << ";" << base_type->str_wrap();

    } else if (c_type == Ctype::Pointer) {
        ss << point_to->str_wrap();
    } else if (c_type == Ctype::Struct) {
        bool first = true;
        for (auto &[name, type] : members) {
            ss << (first ? "" : " | ");
#ifdef VERBOSE
            ss << "<" << type->member_offset << ">";
#endif
            ss << name << ":" << type->str_wrap();
            first = false;
        }
    } else if (c_type == Ctype::Function) {
        bool first = true;
        for (auto &arg : args) {
            ss << (first ? "" : ",") << arg->str_wrap();
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

Type *Type::make_int_type() {
    Type *current       = new Type();
    current->c_type     = Ctype::Plain;
    current->plain_type = PlainType::Int;
    current->align      = 4;
    current->size       = 4;
    return current;
}

Type *Type::make_error_type() {
    Type *current   = new Type();
    current->c_type = Ctype::ErrorType;
    return current;
}

bool Type::is_plain() const { return c_type == Ctype::Plain; }

bool Type::is_int() const {
    if (is_plain() && plain_type == PlainType::Int) {
        return true;
    }
    return false;
}

bool Type::is_arith() const {
    if (is_plain()) {
        return plain_type == PlainType::Int || plain_type == PlainType::Float;
    }
    return false;
}
