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
    Token(string name, string value);

    explicit Token(string name);

    Token(string name, string value, location loc);

    Token(string name, location loc);

    Token();

    // ~Token();

    Token &build_AST(const Token &child);

    string print(const string pre = "", const string ch_pre = "") const;

    string get_name() const;

    void set_name(const string &name);

    string get_value() const;

    optional<location> get_loc() const;

  private:
    string name;
    string value;
    optional<location> loc;
    vector<Token> children;
};
} // namespace Limc

#endif // _TOKEN_H
