#ifndef CJ50_CHAR_H_
#define CJ50_CHAR_H_


#define RESRET(e)                \
    res = (e);                   \
    if (res < 0) { return res; } \
    ret += res;


static UNUSED
void drop_char(const char UNUSED s) { }

static UNUSED
bool equal_char(const char *a, const char *b) {
    return *a == *b;
}


int _print_debug_char(char c) {
    if (c == '"') {
        return printf("\\\"");
    } else if (c == '\\') {
        return printf("\\\\");
    } else if (c == '\0') {
        return printf("\\0");
    } else if (c == '\n') {
        return printf("\\n");
    } else if (c == '\r') {
        return printf("\\r");
    } else if (c == '\t') {
        return printf("\\t");
    } else if (c == '\v') {
        return printf("\\v");
    } else if (c == '\f') {
        return printf("\\f");
    } else if (c == '\b') {
        return printf("\\b");
    } else if (c == '\a') {
        return printf("\\a");
    } else if (iscntrl(c)) {
        return printf("\\%03o", c);
    } else {
        return fputc(c, stdout);
    }
}

int print_debug_char(char c) {
    int ret = 0;
    int res;
    RESRET(fputc('\'', stdout));
    RESRET(_print_debug_char(c));
    RESRET(fputc('\'', stdout));
    return ret;
}

#undef RESRET


GENERATE_Option(char);

#endif /* CJ50_CHAR_H_ */