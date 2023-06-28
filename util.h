#ifndef UTIL_H_
#define UTIL_H_


#define ABORT(...)                              \
    do {                                        \
        fprintf(stderr, __VA_ARGS__);           \
        abort();                                \
    } while(0)



#endif /* UTIL_H_ */
