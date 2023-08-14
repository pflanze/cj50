// parameters: T, VEC, POSSIBLY_CONST


//! Operations on slices only 

//! The `VEC` type is either `slice` or `mutslice`. It is called
//! "slice" from here onwards.


/// Create a new slice from pointer `ptr` and number of elements
/// pointed at `len`. Make sure that `len` is within the range of
/// elements from the pointer! The slice borrows the storage, so make
/// sure its life time is at least the one of the slice.

/// Note that `slice(char)` isn't necessarily holding UTF-8 encoded
/// data! (Todo: add a `str` wrapper which guarantees that.)
static UNUSED
VEC(T) XCAT(new_, VEC(T))(POSSIBLY_CONST T *ptr, size_t len) {
    return (VEC(T)) {
        .ptr = ptr,
        .len = len
    };
}


static UNUSED
void XCAT(drop_, VEC(T))(UNUSED VEC(T) self) { }

