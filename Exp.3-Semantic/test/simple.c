struct A {
    struct {
        int a[10];
    } b[10];
};

void fn() {
    struct A t[10];
    t[0].b[5].a[3];
}