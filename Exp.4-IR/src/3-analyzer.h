#ifndef _ANALYZER_H
#define _ANALYZER_H

#include "0-token.h"
#include "3-symbol.h"
#include "3-type.h"
#include <map>

namespace Limc {

class Driver;

class Semantic {

  public:
    explicit Semantic(Driver &driver);

    void                stmt(Token &root);
    map<string, string> get_str_lit_table();
    map<string, Symbol> get_global_table();

  private:
    Type *expr(Token &root);
    // 对于函数来说，并不需要偏移
    /// TODO 待证实
    void try_insert_symbol(Token &identifier, Type *type, bool is_glb, unsigned offset = 0);

    void enter_scope(const string &name);
    void leave_scope(unsigned frame_size);

    void insert_lib_func(Token &root);

    // 调用C语言的函数
    vector<string>    lib_func_list;
    map<string, bool> lib_func;

    unsigned            string_label = 0;
    map<string, string> string_literal_table;
    // 符号表链，栈顶存放的是当前语句块的符号表
    vector<SymbolTable> tables;
    // 指向目前的顶，用来快速插入
    SymbolTable *current_table;
    // 类型表，存放Struct结构的定义信息
    // <结构体类型名，结构体类型>
    map<string, Type *> type_table;

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
    Type *upper_type(Type *from, Type *to);
    bool  comparable(Type *a, Type *b);
};
} // namespace Limc

#endif // _ANALYZER_H
