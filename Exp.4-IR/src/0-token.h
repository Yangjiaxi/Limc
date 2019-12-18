#ifndef _TOKEN_H
#define _TOKEN_H

#include "3-type.h"
#include "location.hh"
#include <optional>
#include <sstream>
#include <vector>

using namespace std;

namespace Limc {

class Type;

class Token {
  public:
    Token(string kind, string value);

    explicit Token(string name);

    Token(string kind, string value, location loc);

    Token(string kind, location loc);

    Token();

    Token &build_AST(Token child);

    string print(string pre = "", string ch_pre = "") const;

    string get_kind() const;

    void set_kind(const string &kind);

    string get_value() const;

    vector<Token> &get_children();

    Token &get_child(int index);

    optional<location> get_loc() const;

    bool operator==(const string &str) const;

    void   set_type(Type *new_type);
    Type *&get_type();

    void set_func_stack_size(unsigned n);
    void set_offset(unsigned n);
    void set_is_global(bool is_glb);

    unsigned get_offset();
    unsigned get_func_stack_size();

    void replace_childs(vector<Token> new_nodes);

  private:
    string             kind;
    string             value;
    optional<location> loc;
    vector<Token>      children;

    Type *type = nullptr;

    // token的一些属性
    optional<unsigned> func_stack_size; // 函数的栈大小
    /*
     * 偏移
     *      1. 局部变量的定义
     *      2. 取成员操作的目标成员偏移
     *      3. 标识符操作，给出标识符在当前段的偏移
     */
    optional<unsigned> offset;
    optional<bool>     is_global; // 变量是否是全局
};
} // namespace Limc

#endif // _TOKEN_H
