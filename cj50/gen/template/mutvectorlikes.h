// parameters: T, VEC

//! Part of the [`cj50/gen/Vec.h`](../Vec.h.md) library: the parts
//! instantiated once per mutable vectorlike (Vec, mutslice).


/// Move the given `value` into the slot with the given `index`,
/// replacing the value that was there previously. Aborts if `index`
/// isn't smaller than the current `len` of the vector.

static UNUSED
void XCAT(set_, VEC(T))(VEC(T) *self, size_t index, T value) {
    assert(index < self->len);
    XCAT(drop_, T)(self->ptr[index]);
    self->ptr[index] = value;
}

/// Get a mutable slice of the vectorlike. Aborts for invalid indices. (XX todo:
/// Use get_mutslice_* for a variant that returns failures instead.)

static UNUSED
mutslice(T) XCAT(mutslice_of_, VEC(T))(VEC(T) *self, Range range) {
    assert(range.end >= range.start);
    assert(range.end <= self->len);
    return XCAT(new_mutslice_, T)(self->ptr + range.start,
                                  range.end - range.start);
}


