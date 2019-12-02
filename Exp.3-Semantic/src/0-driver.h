#ifndef _DRIVER_H
#define _DRIVER_H

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

    friend class Parser;
    friend class Scanner;
    friend class Semantic;

  private:
    void add_token(const Token &token);

    void inc_x(int x);
    void inc_y(int y);

    location &get_loc();

    static string gen_error(const string &msg, const location &loc);

    void add_error(const string &msg, const Token &token);
    void add_error(const string &msg, const location &loc);
    void add_error(const string &msg);

    Scanner  scanner;
    Parser   parser;
    Semantic analyzer;

    vector<Token> tokens;
    location      loc;

    stringstream errors;
};
} // namespace Limc

#endif // _DRIVER_H
