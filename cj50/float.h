#ifndef CJ50_FLOAT_H_
#define CJ50_FLOAT_H_

#include "Option.h"

static UNUSED
void drop_float(float UNUSED x) { }

static UNUSED
bool equal_float(const float *a, const float *b) {
    return *a == *b;
}

static UNUSED
int print_debug_float(float x) {
    return printf("%g", x);
}

GENERATE_Option(float);


#endif /* CJ50_FLOAT_H_ */
