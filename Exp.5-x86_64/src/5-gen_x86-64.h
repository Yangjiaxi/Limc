#ifndef GEN_X86_64_SRC_5_GEN_X86_64
#define GEN_X86_64_SRC_5_GEN_X86_64

#include "0-token.h"
#include "3-symbol.h"
#include "4-ir.h"
#include <map>

namespace Limc {

class Driver;

class Code {
  public:
    Code() : code() {}

    // insert tab
    Code &tab() {
        code.append("\t");
        return *this;
    }

    // text
    Code &text(const string &str) {
        code.append(str);
        return *this;
    }

    // separator
    Code &comma() {
        code.append(",");
        tab();
        return *this;
    }

    Code &comment(const string &str) {
        code.append("## ");
        code.append(str);
        return *this;
    }

    string show() { return code; }

    // finish
    void ok() { code.append("\n"); }

  private:
    string code;
};

class GenX86_64 {
    using opt_uint = optional<unsigned>;

  public:
    explicit GenX86_64(Driver &driver);

    vector<Code> gen_x86_64(
        vector<BasicFunc> &funcs_ir, map<string, Symbol> &globals, map<string, string> &str_lits);

  private:
    Driver &     driver;
    vector<Code> asm_box;

    unsigned ret_label = 0;
    string   end_label;

    Code &emit();
    void  emit2(const string &code, const string &arg);
    void  emit3(const string &code, const string &arg1, const string &arg2);
    void  emit_header();
    void  emit_global(map<string, Symbol> &globals, map<string, string> &str_lits);
    void  emit_func(BasicFunc &func);
    void  emit_code(IR &ir);
    void  emit_relation(IR &ir, const string &test_cond);

    static string arg_reg(opt_uint reg_no, opt_uint size = 8);
    static string reg(opt_uint reg_no, opt_uint size = 8);

    static string bp_minus(opt_uint reg);
    static string mem(const string &addr);
    static string mem(opt_uint reg_no);

    void print();
};

} // namespace Limc

#endif // GEN_X86_64_SRC_5_GEN_X86_64
