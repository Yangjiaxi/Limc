struct StringBuffer {
    char buffer[1024];
    int  len;
};

struct Scanner {
    struct StringBuffer input[10];
    int                 max_len;
};

void read() {
    struct Scanner handler;
    int            len = handler.input.len;
}