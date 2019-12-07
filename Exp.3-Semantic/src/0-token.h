#ifndef _TOKEN_H
#define _TOKEN_H

#include "location.hh"
#include <optional>
#include <sstream>
#include <vector>

using namespace std;

namespace Limc {
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

    void    set_type(const string &new_type);
    string &get_type();

  private:
    string             kind;
    string             value;
    optional<location> loc;
    vector<Token>      children;

    string type;
};
} // namespace Limc

#endif // _TOKEN_H
