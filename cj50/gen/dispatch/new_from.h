#pragma once

/// Get the conversion function from type `T1` to type `T2`.

#define new_from_(T2, T1)                                               \
    _Generic(*((T2*)(NULL))                                             \
             , SystemError:                                             \
             _Generic(*((T1*)(NULL))                                    \
                      , SystemError: new_SystemError_from_SystemError   \
                      , UnicodeError: /*FAKE*/new_SystemError_from_SystemError \
                      , DecodingError: /*FAKE*/new_SystemError_from_SystemError \
                 )                                                      \
             , UnicodeError:                                            \
             _Generic(*((T1*)(NULL))                                    \
                      , UnicodeError: new_UnicodeError_from_UnicodeError \
                      , DecodingError: new_UnicodeError_from_DecodingError \
                      , SystemError: new_UnicodeError_from_SystemError  \
                 )                                                      \
        )
// ^ Huh, _Generic requires that *all* branches lead to existing
// functions (used or not) *and* that the type cases in the nested
// _Generic calls exist in *all* of them. Thus using fake converters
// for now.

/// Convert `v` into type `T`.

#define new_from(T, v)                          \
    new_from_(T, typeof(v))(v)

