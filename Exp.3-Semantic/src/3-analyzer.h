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

    void try_insert(const Token &name, const Token &type);
    void try_insert(const Token &name, const string &type);
    void try_insert(const Token &name, const Token &type, const vector<Token> &paramaters);

    static optional<Token> find(const Token &root, const string &name);

    void new_table(const string &name);
    void new_table();
    void leave_table();

    vector<SymbolTable> tables;

    int loops = 0;

    int switchs = 0;

    string inner_return_ty;

    Driver &driver;

  private:
    string gen_type(const Token &type_token);

    void walk_var_decl(const Token &root);
    void walk_func_def(const Token &root);
    void walk_block(const Token &root);

    string check_ty(const Token &root);
    string check_ident(const Token &root);
    string check_binary_expr(const Token &root);
    string check_assignment_expr(const Token &root);
    string check_parenthesis_expr(const Token &root);
    string check_prefix_expr(const Token &root);
    string check_postfix_expr(const Token &root);
    string check_ternary_expr(const Token &root);
    string check_call_expr(const Token &root);
    string check_index_expr(const Token &root);
    string check_array_literal(const Token &root);
};
} // namespace Limc

#endif // _ANALYZER_H
