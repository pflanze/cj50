#pragma once

// Helper macros for using chains of print and similar functions that
// return an int with the size of the output if >= 0 and signal an
// error otherwise. Each call is wrapped with RESRET( ), and at the
// end of the function there must be a label `cleanup` and a `return
// ret` statement.

#define INIT_RESRET                             \
    __label__ cleanup; /* GCC extension */      \
    int ret = 0;                                \
    int res;

#define RESRET(e)                               \
    res = (e);                                  \
    if (res < 0) { ret = res; goto cleanup; }   \
    ret += res;

