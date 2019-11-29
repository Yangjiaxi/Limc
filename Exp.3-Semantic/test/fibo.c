int b, c;
float m, n = b + 10;

int a[100][100][100];

int fibo(int a, int b, int c, int d, int e, int f) {
    if (a == 1 || a == 2)
        return 1;
    return fibo(a - 1) + fibo(a - 2);
}

int main() {
    int c = a[101][100][1];
    int m, n, i;
    i = 1;
    while (i <= m) {
        n = fibo(i);
        i = i + 1;
    }
    return 1;
}
