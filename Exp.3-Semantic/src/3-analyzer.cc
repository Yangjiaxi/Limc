#include "3-analyzer.h"
#include "0-driver.h"

#define DEBUG

using namespace Limc;

Semantic::Semantic(Driver &driver) : driver(driver) {}

void Semantic::walk_stmt(Token &root) {
    auto kind = root.get_kind();
#ifdef DEBUG
    cout << "Walk : `" << kind << "`" << endl;
#endif
    // if(kind == "Program")
}

// Type Semantic::check_type(Token &root) { return Type(); }
