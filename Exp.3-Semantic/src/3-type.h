#ifndef SEMANTIC_SRC_3_TYPE_H_
#define SEMANTIC_SRC_3_TYPE_H_

#include "0-token.h"
#include <memory>
#include <unordered_map>

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
    unordered_map<string, Type> members;

    // Function:
    Type *       return_type;
    vector<Type> args;

    unsigned size;
    unsigned align;

    Type();

    static Type build_type(Token &root, vector<unsigned> array_depth = {});

    string to_string() const;

  private:
    string str_kind() const;
    string str_content() const;

    static vector<unsigned> make_array_depths(Token &root);
};

} // namespace Limc

#endif // SEMANTIC_SRC_3_TYPE_H_
