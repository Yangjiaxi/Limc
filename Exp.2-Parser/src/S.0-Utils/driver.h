#ifndef _DRIVER_H
#define _DRIVER_H

#include "../S.1-Scanner/scanner.h"

namespace Limc {
class Driver {
  public:
    Driver();

    int parse();

    void clear();

    string prettyPrint() const;

    void switchInputStream(istream *is);

    friend class Parser;

    friend class Scanner;

  private:
    void addToken(const Token &token);

    void inc_x(int x);

    void inc_y(int y);

    Limc::location &getLocation();

    Scanner scanner;
    Parser parser;
    vector<Token> tokens;
    Limc::location loc;
};
} // namespace Limc

#endif // _DRIVER_H
