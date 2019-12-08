#ifndef _ANALYZER_H
#define _ANALYZER_H

#include "0-token.h"
#include "3-symbol.h"
#include "3-type.h"
#include <unordered_map>

namespace Limc {

class Driver;

class Semantic {

  public:
    explicit Semantic(Driver &driver);

    void  stmt(Token &root);
    Type *expr(Token &root);

  private:
    // void try_insert(Token &name, Token &type);
    // void try_insert(Token &name, string &type);
    // void try_insert(Token &name, Token &type, vector<Token> &parameters);

    // 对于函数来说，并不需要偏移
    /// TODO 待证实
    void try_insert_symbol(Token &identifier, Type *type, unsigned offset = 0);

    void enter_scope(const string &name);
    // void enter_scope();
    void leave_scope();

    // 符号表链，栈顶存放的是当前语句块的符号表
    vector<SymbolTable> tables;
    // 指向目前的顶，用来快速插入
    SymbolTable *current_table;
    // 类型表，存放Struct结构的定义信息
    // <结构体类型名，结构体类型>
    unordered_map<string, Type *> type_table;

    // 全局存储区大小
    unsigned global_stack_size = 0;

    // 当前函数块的语句大小，包括函数参数列表变量的大小与函数体中的所有局部声明
    unsigned local_stack_size = 0;

    int loops = 0;

    // string inner_return_ty;

    Type *inner_return_ty;

    Driver &driver;

  private:
    Type *parse_type(Token &root);
    bool  can_convert(Type *from, Type *to);
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
