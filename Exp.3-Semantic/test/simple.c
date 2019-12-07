struct T {
    int a[3];
    struct {
        int b[10];
    } p[5][3];
};

int main() {
    struct T s[10];
    s[1].a[0] = 1;
    s[2].p[0][1].b[5] = 12;
    return 0;
}