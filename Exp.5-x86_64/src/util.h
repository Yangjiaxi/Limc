#ifndef SEMANTIC_SRC_UTIL_H_
#define SEMANTIC_SRC_UTIL_H_

#include "0-color.h"
#include <iomanip>

// 全局定义
constexpr unsigned REG_NUM = 7;
constexpr unsigned ARG_REG_NUM = 6;

// 8|4|1字节通用寄存器
constexpr char REG64[][REG_NUM] = {
    "r10", "r11", "rbx", "r12", "r13", "r14", "r15"};
constexpr char REG32[][REG_NUM] = {
    "r10d", "r11d", "ebx", "r12d", "r13d", "r14d", "r15d"};
constexpr char REG8[][REG_NUM] = {
    "r10b", "r11b", "bl", "r12b", "r13b", "r14b", "r15b"};

// 8|4|1字节参数寄存器
constexpr char ARG_REG64[][ARG_REG_NUM] = {
    "rdi", "rsi", "rdx", "rcx", "r8", "r9"};
constexpr char ARG_REG32[][ARG_REG_NUM] = {
    "edi", "esi", "edx", "ecx", "r8d", "r9d"};
constexpr char ARG_REG8[][ARG_REG_NUM] = {
    "dil", "sil", "dl", "cl", "r8b", "r9b"};

static string repeat_str(const string &ref, unsigned n) {
    stringstream ss;
    for (unsigned i = 0; i < n; ++i) {
        ss << ref;
    }
    return ss.str();
}

template <typename T> string opt(optional<T> &ref) {
    if (ref == nullopt) {
        return "None";
    } else {
        return to_string(ref.value());
    }
}

static stringstream &build_str(stringstream &ss,
                               const string &value,
                               const char *color,
                               const int width = 0) {
    if (width)
        ss << color << setw(width) << value << RESET_COLOR;
    else
        ss << color << value << RESET_COLOR;
    return ss;
}

static stringstream &
build_str(stringstream &ss, const string &value, const int width = 0) {
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
