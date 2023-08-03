// This is a template include file. It expects `T` to be defined.


// COPY-PASTE from cj50.h  except INIT_RESRET removed from PRINT_ARRAY

#define INIT_RESRET                             \
    __label__ cleanup; /* GCC extension */      \
    int ret = 0;                                \
    int res;

#define RESRET(e)                               \
    res = (e);                                  \
    if (res < 0) { ret = res; goto cleanup; }   \
    ret += res;

#define PRINT_ARRAY(print_typ, ary, len)        \
    RESRET(print_cstr("{"));                    \
    for (size_t i = 0; i < len; i++) {          \
        if (i > 0) {                            \
            RESRET(print_cstr(", "));           \
        }                                       \
        RESRET(print_typ(&ary[i]));             \
    }                                           \
    RESRET(print_cstr("}"));                    \
cleanup:                                        \
    return ret;

// /COPY-PASTE from cj50.h


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

// Appends an element to the back of the vector.
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

static UNUSED
size_t XCAT(len_, Vec(T))(const Vec(T) *self) {
    return self->len;
}

static UNUSED
size_t XCAT(is_empty_, Vec(T))(const Vec(T) *self) {
    return self->len == 0;
}

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

static UNUSED
int XCAT(print_debug_, Vec(T))(const Vec(T) *self) {
    INIT_RESRET;
    size_t len = self->len;
    const T *ptr = self->ptr;
    PRINT_ARRAY(XCAT(print_debug_, T), ptr, len);
}

static UNUSED
int XCAT(print_debug_move_, Vec(T))(Vec(T) self) {
    int res = XCAT(print_debug_, Vec(T))(&self);
    XCAT(drop_, Vec(T))(self);
    return res;
}



#undef PRINT_ARRAY
#undef RESRET
#undef INIT_RESRET
