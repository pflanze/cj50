#pragma once

/// Get the conversion function from T1 to T2.

#define new_from(T2, T1)                                                \
    _Generic(*((T2*)(NULL))                                             \
             , UnicodeError:                                            \
             _Generic(*((T1*)(NULL))                                    \
                      , DecodingError: new_UnicodeError_from_DecodingError \
                      , SystemError: new_UnicodeError_from_SystemError  \
                 )                                                      \
        )


