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
    void show_irs();

  private:
    Driver &    driver;
    IRContainer ir_box;

    int current_temp  = 0;
    int current_label = 0;

    vector<string> current_break_labels;
    vector<string> current_continue_labels;

    string current_return_label;

    string generate_temp();
    string generate_label();
};

} // namespace Limc

#endif // IR_4_GEN_IR_H_
