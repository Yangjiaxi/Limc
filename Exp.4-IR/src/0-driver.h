#ifndef _DRIVER_H
#define _DRIVER_H

#include "0-report.h"
#include "0-token.h"
#include "1-scanner.h"
#include "3-analyzer.h"
#include "4-gen_ir.h"
#include <map>

using std::vector;

namespace Limc {

class Driver {
  public:
    Driver();

    bool parse();
    bool analyze();
    void gen_ir();
    void print_ir();

    void alloc_reg();
    void optimize();
    void gen_x86_64();

    void clear();

    string print() const;
    void   set_entry(istream *is);
    void   print_reports();
    Token &get_root();

    friend class Parser;
    friend class Scanner;
    friend class Semantic;
    friend class GenIR;

  private:
    void add_token(const Token &token);

    void inc_x(int x);
    void inc_y(int y);

    location &get_loc();

    Report &report();

    Scanner  scanner;
    Parser   parser;
    Semantic analyzer;
    GenIR    ir_maker;

    vector<Token> tokens;
    location      loc;

    vector<Report> reports;

    vector<string> input_file;

    vector<BasicFunc>   funcs_ir;
    map<string, Symbol> globals;
    map<string, string> str_lits;

    bool parse_ok   = false;
    bool analyze_ok = false;
};
} // namespace Limc

#endif // _DRIVER_H
