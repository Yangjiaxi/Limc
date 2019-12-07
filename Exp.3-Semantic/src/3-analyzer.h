#ifndef _ANALYZER_H
#define _ANALYZER_H

#include "0-token.h"
#include "3-symbol.h"
#include "3-type.h"

namespace Limc {

class Driver;

class Semantic {

  public:
    explicit Semantic(Driver &driver);

    void walk_stmt(Token &root);
    Type check_type(Token &root);

  private:
    // void try_insert(Token &name, Token &type);
    // void try_insert(Token &name, string &type);
    // void try_insert(Token &name, Token &type, vector<Token> &parameters);

    // void new_table(const string &name);
    // void new_table();
    // void leave_table();

    // 符号表
    vector<SymbolTable> tables;
    // 类型表，存放Struct结构的定义信息
    vector<Type> type_table;

    int loops = 0;

    string inner_return_ty;

    Driver &driver;

  private:
    // void walk_var_decl(Token &root);
    // void walk_func_def(Token &root);
    // void walk_block(Token &root);
    //
    // string check_ty(Token &root);
    // string check_ident(Token &root);
    // string check_binary_expr(Token &root);
    // string check_relational_expr(Token &root);
    // string check_assignment_expr(Token &root);
    // string check_parenthesis_expr(Token &root);
    // string check_prefix_expr(Token &root);
    // string check_postfix_expr(Token &root);
    // string check_ternary_expr(Token &root);
    // string check_call_expr(Token &root);
    // string check_index_expr(Token &root);
    // string check_array_literal(Token &root);
};
} // namespace Limc

#endif // _ANALYZER_H
