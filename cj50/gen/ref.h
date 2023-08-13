#pragma once

//! `ref(T)` simply represents a constant reference to `T`,
//! i.e. `const T*`. This exists so that references can be made part
//! of parametrized data types like `Option` (because `T*` would not
//! work because the `*` can't be part of the type name that
//! `Option(T*)` would generate).


#include <cj50/basic-util.h>
#include <cj50/macro-util.h>


/// This macro creates a type name for an `Option` specific for the
/// given type name `T`.

/// Implementation wise, it simply concatenates `ref_` and the given
/// type name. For this reason, the type name `T` must not contain
/// spaces or `*`, e.g. `unsigned int` would not work and a typedef
/// like `uint` has to be used instead. For handling references, see
/// the `ref(T)` type generator (OK that's us).

#define ref(T) XCAT(ref_,T)

/// This macro defines the type `ref(T)`. It has to be used once for a
/// given type `T`. Afterwards `ref(T)` can be used any number of
/// times.
#define GENERATE_ref(T)                         \
    typedef const T* ref(T)

