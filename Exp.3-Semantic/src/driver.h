#ifndef _DRIVER_H
#define _DRIVER_H

#include "scanner.h"

namespace Limc {
class Driver {
  public:
    Driver();

    int parse();

    void clear();

    string print() const;

    void switchInputStream(istream *is);

    friend class Parser;

    friend class Scanner;

  private:
    void add_token(const Token &token);

    void inc_x(int x);

    void inc_y(int y);

    location &get_loc();

    Scanner scanner;
    Parser parser;
    // Analyzer analyzer;

    vector<Token> tokens;
    location loc;
};
} // namespace Limc

#endif // _DRIVER_H
