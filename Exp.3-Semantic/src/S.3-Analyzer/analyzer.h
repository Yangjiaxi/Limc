#ifndef _ANALYZER_H
#define _ANALYZER_H

#include "../S.0-Utils/driver.h"
#include "../S.0-Utils/token.h"
#include "symbol.h"

namespace Limc {

class SemanticAnalyzer {
  public:
    explicit SemanticAnalyzer(Driver &driver);

    void analyze(const Token &root);

  private:
    static runtime_error semanticError(const string &message,
                                       const Token &token);

    string checkType(const Token &token);

    void insert(const Token &name, const Token &type);

    void insert(const Token &name, const string &type);

    void insert(const Token &name, const Token &type,
                const vector<Token> &arguments);

    static optional<Token> find(const Token &root, const string &name);

    vector<SymbolTable> tables;

    int loopDepth = 0;

    int switchDepth = 0;

    string currentReturn;

    Driver &driver;
};
} // namespace Limc

#endif // _ANALYZER_H
