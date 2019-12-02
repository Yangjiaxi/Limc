struct ex {
    int   a;
    float f;
    struct {
        int  b;
        char c;
    } p;
};

void fn() {
    struct ex v = {1, 1.1, {1, 'c'}};
    v.p.b       = 12;
}