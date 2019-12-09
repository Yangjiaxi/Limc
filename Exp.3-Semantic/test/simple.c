struct A {
    struct {
        int a[10];
    } b[10];
};

void fn(struct A p[10]) {
    // tt
    p[5].b[3].a++;
}