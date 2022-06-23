#ifndef SEMANTIC_SRC_UTIL_H_
#define SEMANTIC_SRC_UTIL_H_

#include "0-color.h"
#include <iomanip>

static stringstream &
build_str(stringstream &ss, const string &value, const char *color, const int width = 0) {
    if (width)
        ss << color << setw(width) << value << RESET_COLOR;
    else
        ss << color << value << RESET_COLOR;
    return ss;
}

static stringstream &build_str(stringstream &ss, const string &value, const int width = 0) {
    if (width)
        ss << setw(width) << value << RESET_COLOR;
    else
        ss << value << RESET_COLOR;
    return ss;
}

static unsigned align_memory(unsigned base, unsigned align) {
    return (base + align - 1) & ~(align - 1);
}

#endif // SEMANTIC_SRC_UTIL_H_
