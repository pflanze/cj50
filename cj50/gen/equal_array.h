#pragma once

//! Check for structural equality between arrays.

//! Because arrays are used ad hoc and don't have their own
//! GENERATE_.. macro, this is needed separately.


/// Generate a function that checks for structural equality of two
/// arrays, specialized for a given type T. Call this macro once, then
/// the following function exists:

/// ```C
/// equal_array_T(T* a, size_t alen,
///               T* b, size_t blen)
/// ```

/// Also see the `equal_array` generic function that, if (currently
/// manually) updated to include the newly defined one, will call
/// that.

#define GENERATE_equal_array(T)                         \
    static UNUSED                                       \
    bool XCAT(equal_array_, T)(T* a, size_t alen,       \
                               T* b, size_t blen) {     \
        if (alen == blen) {                             \
            for (size_t i = 0; i < alen; i++) {         \
                if (! XCAT(equal_, T)(&a[i], &b[i])) {  \
                    return false;                       \
                }                                       \
            }                                           \
            return true;                                \
        } else {                                        \
            return false;                               \
        }                                               \
    }                                                   \


