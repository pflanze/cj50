// parameters: T, VEC

//! Part of the [`cj50/gen/Vec.h`](../Vec.h.md) library: Operations on
//! slices and `Vec`s.

//! The `VEC` type is either `Vec`, `slice` or `mutslice`. It is
//! called "vectorlike" from here onwards.

#include <cj50/Range.h>


// COPY-PASTE from cj50.h  except INIT_RESRET removed from PRINT_ARRAY

// Also, have to use prefix, as there is no nesting. Stupid.

#define VEC_PRINT_ARRAY(print_typ, ary, len)    \
    RESRET(print_move_cstr("{"));               \
    for (size_t i = 0; i < len; i++) {          \
        if (i > 0) {                            \
            RESRET(print_move_cstr(", "));      \
        }                                       \
        RESRET(print_typ(&ary[i]));             \
    }                                           \
    RESRET(print_move_cstr("}"));               \
cleanup:                                        \
    return ret;

// /COPY-PASTE from cj50.h


// XX Todo: to save on code size, should use delegates for some of
// these (the larger functions) instead. Vec -> delegate to function
// on mutslice or slice. mutslice -> delegate to function on slice.



/// The number of elements the vectorlike is currently holding.
static UNUSED
size_t XCAT(len_, VEC(T))(const VEC(T) *self) {
    return self->len;
}

/// Whether the vectorlike has exactly 0 elements.
static UNUSED
size_t XCAT(is_empty_, VEC(T))(const VEC(T) *self) {
    return self->len == 0;
}

/// Whether the two vectorlikes have the same number of elements with equal
/// elements in every position.
static UNUSED
bool XCAT(equal_, VEC(T))(const VEC(T) *a, const VEC(T) *b) {
    size_t len = a->len;
    return (len == b->len) && ({
            bool res = true;
            const T* av = a->ptr;
            const T* bv = b->ptr;
            for (size_t i = 0; i < len; i++) {
                if (! XCAT(equal_, T)(&av[i], &bv[i])) {
                    res = false;
                    break;
                }
            }
            res;
        });
}


/// Print in C code syntax.
static UNUSED
int XCAT(print_debug_, VEC(T))(const VEC(T) *self) {
    INIT_RESRET;
    size_t len = self->len;
    const T *ptr = self->ptr;
    VEC_PRINT_ARRAY(XCAT(print_debug_, T), ptr, len);
}

/// Print in C code syntax, consuming the argument.
static UNUSED
int XCAT(print_debug_move_, VEC(T))(VEC(T) self) {
    int res = XCAT(print_debug_, VEC(T))(&self);
    XCAT(drop_, VEC(T))(self);
    return res;
}


/// Get a reference to the element at position `i`. Aborts if `i` is
/// behind the end of the vectorlike. (XX todo: If you are not sure if
/// `i` is valid, use `get` instead.)

static UNUSED
const T* XCAT(at_, VEC(T))(const VEC(T) *self, size_t idx) {
    assert(idx < self->len);
    return &self->ptr[idx];
}


/// Get a slice of the vectorlike. Aborts for invalid indices. (XX todo:
/// Use get_slice_* for a variant that returns failures instead.)

static UNUSED
slice(T) XCAT(slice_of_, VEC(T))(const VEC(T) *self, Range range) {
    assert(range.end >= range.start);
    assert(range.end <= self->len);
    return XCAT(new_slice_, T)(self->ptr + range.start,
                               range.end - range.start);
}



#undef VEC_PRINT_ARRAY
