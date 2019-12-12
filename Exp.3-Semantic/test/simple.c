struct {
    int a;
    char b;
    int c[10];
    char d;
    struct {
        int a[10];
        char c[5][5];
    } s[5];
} tt;

int a;

int main() {
    int a = tt.a;
    float b = tt.b;
    tt.c;
    tt.d;
    return 0;
}