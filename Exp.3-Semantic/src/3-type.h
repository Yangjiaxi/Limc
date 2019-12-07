#ifndef SEMANTIC_SRC_3_TYPE_H_
#define SEMANTIC_SRC_3_TYPE_H_

#include "0-token.h"
#include <memory>
// #include <unordered_map>
#include <map>

/*
 * 节点的大类型：(Type)
 *  1. 类型
 *  2. 大小 (sizeof)
 *  3. 对齐属性 (alignof)
 *
 * 节点的小类型：(Ctype)
 *  ----- Plain types -----
 *  1. Int
 *  2. Float
 *  3. Char
 *  4. Void
 *  ----- Complex -----
 *  5. Array
 *      - Type (Element Type)
 *      - len
 *  6. Struct
 *      - Vector<Type, name>
 *  7. Function
 *      - Type (return)
 *      - Vector<Type> (args)
 */

using namespace std;

namespace Limc {

enum Ctype { Plain, Array, Struct, Function };
enum PlainType { Int, Float, Char, Void };

class Type {
  public:
    Ctype c_type;

    // Plain:
    PlainType plain_type;

    // Array:
    Type *   base_type;
    unsigned length;

    // Struct:
    map<string, Type> members;

    // for struct element
    unsigned member_offset;

    // Function:
    Type *       return_type;
    vector<Type> args;

    // array, struct,
    unsigned size;
    unsigned align;

    Type();

    static Type build_type(Token &root, const vector<unsigned> &array_depth = {});

    string to_string() const;

  private:
    string str_content() const;

    static vector<unsigned> make_array_depths(Token &root);

    // size, align
    static pair<unsigned, unsigned> make_array_info(Type *root);
};

} // namespace Limc

#endif // SEMANTIC_SRC_3_TYPE_H_
