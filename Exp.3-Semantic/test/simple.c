int a;

int a(float b) {
    int m = (b ? 1 : 2);
    return c + b[10] + a[1.2];
}

struct {
    int a;
    struct {
        int b;
    } p;
} pp;

int main() {
    // cmt
    return pp.a + pp.p.b;
}