#ifndef _TOKEN_H
#define _TOKEN_H

#include <string>
#include <vector>

using namespace std;

namespace Limc {
class Token {
  public:
    Token(string name, string value);

    Token(string name);

    Token();

    ~Token();

    Token &build_AST(const Token &child);

    string pretty_print(const string prefix = "",
                        const string child_prefix = "") const;

    string get_name() const;

    string get_value() const;

  private:
    string name;
    string value;
    vector<Token> children;
};
} // namespace Limc

#endif // _TOKEN_H
