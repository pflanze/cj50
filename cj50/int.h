#ifndef CJ50_INT_H_
#define CJ50_INT_H_

#include <cj50/gen/Option.h>

static UNUSED
bool equal_int(const int *a, const int *b) {
    return *a == *b;
}

static UNUSED
void drop_int(const int UNUSED a) {}

static UNUSED
int print_debug_int(int n) {
    return printf("%i", n);
}

GENERATE_Option(int);


#endif /* CJ50_INT_H_ */
