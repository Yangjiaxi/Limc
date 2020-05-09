use printf;
use scanf;

int main() {
    int a, b;
    printf("[a^b] Input a b: ");
    scanf("%d%d", &a, &b);
    int res = fast_pow(a, b);
    printf("%d ^ %d = %d\n", a, b, res);
    return 0;
}
