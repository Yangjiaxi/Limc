#ifndef GEN_X86_64_SRC_5_REG_ALLOC_H_
#define GEN_X86_64_SRC_5_REG_ALLOC_H_

#include "4-ir.h"

namespace Limc {

class Driver;

class RegAllocator {
    using opt_uint = optional<unsigned>;

  public:
    explicit RegAllocator(Driver &driver);
    void reg_alloc(vector<BasicFunc> &funcs);

  private:
    Driver &driver;

    void alloc_ir(vector<IR> &ir);
    unsigned map_reg(unsigned fake_reg);

    vector<opt_uint> prev_mapped_regs;
    vector<bool> current_in_use;
};

} // namespace Limc

#endif
