struct A {
    int a;
    struct {
        float s;
        char c;
    } b;
    int c;
};


int main() {
    struct A p1 = {1, {1.1}, 1};
    return 0;
}