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

    Token &build_AST(const Token &child);

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

  private:
    string             kind;
    string             value;
    optional<location> loc;
    vector<Token>      children;

    optional<unsigned> func_stack_size;

    Type *type = nullptr;
};
} // namespace Limc

#endif // _TOKEN_H
