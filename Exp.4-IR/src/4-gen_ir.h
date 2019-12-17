#ifndef IR_4_GEN_IR_H_
#define IR_4_GEN_IR_H_

#include "0-token.h"
#include "4-ir.h"

namespace Limc {

class Driver;

class GenIR {
  public:
    explicit GenIR(Driver &driver);

    string gen_expr(Token &root);
    string gen_stmt(Token &root);

  private:
    Driver &    driver;
    IRContainer ir_box;
};

} // namespace Limc

#endif // IR_4_GEN_IR_H_
