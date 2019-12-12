struct A {
    int  a[10];
    char c[5][5];
};

struct B {
    int      a;
    char     b;
    int      c[10];
    char     d;
    struct A s[5];
};

struct B p[10];

int a;

int main() {
    return p[2].s[2].c[2][2];
}

