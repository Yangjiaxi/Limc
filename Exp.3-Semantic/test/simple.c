struct A {
    struct {
        int a[10];
    } b[10];
};

int fn(struct A p[10]) {
    // tt
    return p[5].b[3].a[0]++;
}