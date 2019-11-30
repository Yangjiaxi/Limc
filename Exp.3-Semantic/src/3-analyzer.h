#ifndef _ANALYZER_H
#define _ANALYZER_H

#include "0-token.h"
#include "3-symbol.h"

namespace Limc {

class Driver;

class Semantic {

  public:
    explicit Semantic(Driver &driver);

    void walk(const Token &root);

  private:
    static runtime_error semantic_error(const string &msg, const Token &token);

    string check_ty(const Token &token);

    void try_insert(const Token &name, const Token &type);

    void try_insert(const Token &name, const string &type);

    void try_insert(const Token &name, const Token &type, const vector<Token> &paramaters);

    static optional<Token> find(const Token &root, const string &name);

    vector<SymbolTable> tables;

    int loops = 0;

    int switchs = 0;

    string inner_return_ty;

    Driver &driver;
};
} // namespace Limc

#endif // _ANALYZER_H
