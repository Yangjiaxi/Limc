#ifndef IR_4_IR_H_
#define IR_4_IR_H_

#include "0-token.h"

namespace Limc {

class Driver;

class IR {
  public:
    IR();

    IR &set_op(string _op);
    IR &set_arg1(string _arg1);
    IR &set_arg2(string _arg2);
    IR &set_res(string _res);

    string to_string() const;

  private:
    string op;
    string arg1;
    string arg2;
    string res;
};

class IRContainer {
  public:
    explicit IRContainer();

    IR &append();

    void show();

    unsigned size() const;

    IR &operator[](unsigned i);

  private:
    vector<IR> container;

    static void show_head();
};

} // namespace Limc

#endif // IR_4_HIR_H_
