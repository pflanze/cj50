// This is a template include file. It expects `T` to be defined.



/// A `slice` consists of two fields, a pointer to an array, and the
/// current length used out of that array. A slice is borrowing the
/// storage it is representing (either from the `Vec` it is taken from
/// (meaning, you can't modify or move the `Vec` while any `slice` of
/// it is in use), or from static storage).

typedef struct slice(T) {
    const T *ptr;
    const size_t len;
} slice(T);


#define VEC slice
#include <cj50/gen/template/slices.h>
#include <cj50/gen/template/vectorlikes.h>
#undef VEC



/// A `mutslice` is like a `slice` but allows mutation of the items in
/// the slice. Only maximally one `mutslice` to the same storage
/// should exist at any time to avoid the risk of concurrent mutation.

typedef struct mutslice(T) {
    T *ptr;
    const size_t len; // still leave the length immutable
} mutslice(T);


#define VEC mutslice
#include <cj50/gen/template/slices.h>
#include <cj50/gen/template/vectorlikes.h>
#undef VEC



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

#define VEC Vec
#include <cj50/gen/template/vectorlikes.h>
#undef VEC


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

/// Clears the vector, removing all values.

/// Note that this method has no effect on the allocated capacity of
/// the vector.

static UNUSED
void XCAT(clear_, Vec(T))(Vec(T) *self) {
    size_t len = self->len;
    T* ptr = self->ptr;
    if (ptr) {
        for (size_t i = 0; i < len; i++) {
            XCAT(drop_, T)(ptr[i]);
        }
    } else {
        assert(len == 0);
    }
    self->len = 0;
}


