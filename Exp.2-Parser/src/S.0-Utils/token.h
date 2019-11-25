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

    void buildAST(const Token &child);

    string prettyPrint(const string prefix = "",
                       const string childPrefix = "") const;

    string getName() const;

    string getValue() const;

  private:
    string name;
    string value;
    vector<Token> children;
};
} // namespace Limc

#endif // _TOKEN_H
