struct A {
    int a;
};

struct B {
    char     a;
    struct A b[5];
    int      c;
} t1;

int main() {
    struct B p[5];
    int      p1 = p[0].a;
    int      p2 = t1.b[2].a;
    return p1 + p2;
}