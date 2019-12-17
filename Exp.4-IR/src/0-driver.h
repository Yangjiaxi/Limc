#ifndef _DRIVER_H
#define _DRIVER_H

#include "0-report.h"
#include "0-token.h"
#include "1-scanner.h"
#include "3-analyzer.h"
using std::vector;

namespace Limc {

class Driver {
  public:
    Driver();

    int  parse();
    void analyze();

    void clear();

    string print() const;
    void   set_entry(istream *is);
    void   print_reports();
    Token &get_root();

    friend class Parser;
    friend class Scanner;
    friend class Semantic;

  private:
    void add_token(const Token &token);

    void inc_x(int x);
    void inc_y(int y);

    location &get_loc();

    Report &report();

    Scanner  scanner;
    Parser   parser;
    Semantic analyzer;

    vector<Token> tokens;
    location      loc;

    vector<Report> reports;

    vector<string> input_file;
};
} // namespace Limc

#endif // _DRIVER_H
