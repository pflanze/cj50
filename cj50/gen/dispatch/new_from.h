#pragma once

/// Get the conversion function from type `T1` to type `T2`.

#define new_from_(T2, T1)                                               \
    _Generic(*((T2*)(NULL))                                             \
             , UnicodeError:                                            \
             _Generic(*((T1*)(NULL))                                    \
                      , DecodingError: new_UnicodeError_from_DecodingError \
                      , SystemError: new_UnicodeError_from_SystemError  \
                 )                                                      \
        )


/// Convert `v` into type `T`.

#define new_from(T, v)                          \
    new_from_(T, typeof(v))(v)

