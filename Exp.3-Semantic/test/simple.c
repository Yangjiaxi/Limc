char *yes = "yes";
char *no  = "no";
char *big(float a, float b) { return (a > b ? yes : no); }

int max(int a, int b) { return (a > b ? a : b); }

void write(char **s) { return; }

int main() {
    float a = 10.1;
    float b = 1;
    char  c = a[5];
    int   t = *a;
    write(big(a, b));
    break;
    continue;
    return max(a, b);
}