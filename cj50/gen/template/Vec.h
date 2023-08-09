// This is a template include file. It expects `T` to be defined.


// COPY-PASTE from cj50.h  except INIT_RESRET removed from PRINT_ARRAY

// Also, have to use prefix, as there is no nesting. Stupid.

#define Vec_PRINT_ARRAY(print_typ, ary, len)    \
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


/// A Vec consists of three fields, a pointer to a heap-allocated
/// array, the current size of that array, and the current length used
/// out of that array.

/// Never mutate those fields directly, use accessor functions
/// instead!

typedef struct Vec(T) {
    T *ptr;
    size_t cap;
    size_t len;
} Vec(T);


/// Construct a new, empty vector
static UNUSED
Vec(T) XCAT(new_, Vec(T))() {
    return (Vec(T)) {
        // (We're not currently doing Option optimization, so can use
        // NULL to indicate no space allocation.)
        .ptr = NULL,
        .cap = 0,
        .len = 0
    };
}

/// Construct a new, empty vector with at least the specified capacity.
static UNUSED
Vec(T) XCAT(with_capacity_, Vec(T))(size_t cap) {
    return (Vec(T)) {
        .ptr = xcallocarray(cap, sizeof(T)),
        .cap = cap,
        .len = 0
    };
}

/// Appends an element to the back of the vector if there's still
/// capacity left for it, otherwise returns a
/// `VecError_out_of_capacity` error. I.e. never allocates additional
/// memory.
static UNUSED
Result(Unit, VecError) XCAT(push_within_capacity_, Vec(T))(
    Vec(T) *self, T value)
{
    size_t len = self->len;
    size_t cap = self->cap;
    if (len < cap) {
        self->ptr[len] = value;
        self->len = len + 1;
        return Ok(Unit, VecError)(Unit());
    } else {
        return Err(Unit, VecError)(VecError_out_of_capacity);
    }
}

/// Reserve space for `additional` more elements on top of the current
/// capacity (not len).
static UNUSED
void XCAT(reserve_, Vec(T))(Vec(T) *self, size_t additional) {
    // Does the Rust version add `additional` to cap or to len?
    // Actually cap, since RawVec doesn't even *have* len.
    size_t cap = self->cap;
    size_t cap2 = cap + additional;
    assert(cap2 > cap);
    void* ptr2 = xreallocarray(self->ptr, cap2, sizeof(T));
    self->ptr = ptr2;
    self->cap = cap2;
}

/// Appends an element to the back of the vector.
static UNUSED
void XCAT(push_, Vec(T))(Vec(T) *self, T value) {
    AUTO res = XCAT(push_within_capacity_, Vec(T))(self, value);
    if (res.is_ok) {
        return;
    }
    XCAT(reserve_, Vec(T))(self, max_size_t(8, self->cap));
    unwrap_Result_Unit__VecError(
        XCAT(push_within_capacity_, Vec(T))(self, value));
    // ^ could just assert(res.is_ok) instead 
}

/// Removes the last element from a vector and returns it, or None if
/// it is empty.
static UNUSED
Option(T) XCAT(pop_, Vec(T))(Vec(T) *self) {
    size_t len = self->len;
    if (len > 0) {
        size_t len2 = len - 1;
        T val = self->ptr[len2];
        self->len = len2;
        return XCAT(some_, T)(val);
    } else {
        return XCAT(none_, T)();
    }
}


/// Moves all the elements of `other` into `self`, leaving `other` empty.
static UNUSED
void XCAT(append_, Vec(T))(Vec(T) *self, Vec(T) *other) {
    size_t count = other->len;
    XCAT(reserve_, Vec(T))(self, count);
    size_t len = self->len;
    size_t len2 = len + count;
    assert(len2 > len);
    // XX multiplication overflow check?
    memcpy(&self->ptr[len], other->ptr, count * sizeof(T));
    self->len = len2;
    other->len = 0;
}

/// The number of elements the Vec is currently holding.
static UNUSED
size_t XCAT(len_, Vec(T))(const Vec(T) *self) {
    return self->len;
}

/// Whether the Vec has exactly 0 elements.
static UNUSED
size_t XCAT(is_empty_, Vec(T))(const Vec(T) *self) {
    return self->len == 0;
}

/// Remove from existence, along with the owned elements.
static UNUSED
void XCAT(drop_, Vec(T))(Vec(T) self) {
    size_t len = self.len;
    T* ptr = self.ptr;
    if (ptr) {
        for (size_t i = 0; i < len; i++) {
            XCAT(drop_, T)(ptr[i]);
        }
        free(ptr);
    } else {
        assert(len == 0);
    }
}

/// Whether the two vectors have the same number of elements with
/// equal elements in every position.
static UNUSED
bool XCAT(equal_, Vec(T))(const Vec(T) *a, const Vec(T) *b) {
    size_t len = a->len;
    return (len == b->len) && ({
            bool res = true;
            T* av = a->ptr;
            T* bv = b->ptr;
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
int XCAT(print_debug_, Vec(T))(const Vec(T) *self) {
    INIT_RESRET;
    size_t len = self->len;
    const T *ptr = self->ptr;
    Vec_PRINT_ARRAY(XCAT(print_debug_, T), ptr, len);
}

/// Print in C code syntax, consuming the argument.
static UNUSED
int XCAT(print_debug_move_, Vec(T))(Vec(T) self) {
    int res = XCAT(print_debug_, Vec(T))(&self);
    XCAT(drop_, Vec(T))(self);
    return res;
}



#undef Vec_PRINT_ARRAY
