#ifndef IR_4_GEN_IR_H_
#define IR_4_GEN_IR_H_

#include "0-token.h"
#include "4-ir.h"

namespace Limc {

class Driver;

class GenIR {
  public:
    explicit GenIR(Driver &driver);

    vector<BasicFunc> gen_ir(Token &token);

    void add()

    void store_param(unsigned size, unsigned bp_offset, unsigned arg_reg);


  private:
    Driver &          driver;
    vector<IR>        codes;
    vector<BasicFunc> funcs;
};

} // namespace Limc

#endif // IR_4_GEN_IR_H_
