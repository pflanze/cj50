#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>

typedef char* string;

string get_string() {
    char *line = NULL;
    size_t len = 0;
    errno = 0;
    ssize_t n = getline(&line, &len, stdin);
    if (n < 0) {
        fprintf(stderr,
                "Could not get a line from stdin: %s.\n",
                errno == 0 ? "EOF" : strerror(errno));
        abort();
    }
    size_t l = strlen(line);
    assert(l >= 1);
    line[l - 1] = '\0';
    return line;
}

void print_string(const char* str) {
    printf("%s", str);
}

int get_int() {
    while (true) {
        string s = get_string();
        char *tail;
        errno = 0;
        long n = strtol(s, &tail, 10);
        if (errno == 0) {
            if (*tail == '\0') {
                if (n >= INT_MIN && n < INT_MAX) {
                    free(s);
                    return n;
                } else {
                    print_string("Your answer is not within the range of numbers of the `int` type.");
                }
            } else {
                print_string("Please enter only an integer number with nothing after it.");
            }
        } else {
            printf("Your answer is not an integer number in range possible for the `long` type: %s.",
                   strerror(errno));
        }
        free(s);
        print_string(" Please enter an integer number: ");
    }
}

void print_int(int n) {
    printf("%i", n);
}


typedef unsigned int nat;

nat get_nat() {
    while (true) {
        int i = get_int();
        if (i > 0) {
            return i;
        }
        print_string("Your answer is not above zero.");
        print_string(" Please enter a natural number: ");
    }
}

void print_nat(nat n) {
    printf("%u", n);
}

