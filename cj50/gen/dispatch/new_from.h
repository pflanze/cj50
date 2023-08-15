
// Huh, _Generic requires that *all* branches lead to existing
// functions (used or not) *and* that the type cases in the nested
// _Generic calls exist in *all* of them. Thus using fake converters.

// Allow to re-import to add more parts
#ifndef NEW_FROM_h
#define NEW_FROM_h

#undef new_from_
#undef new_from
#ifndef NEW_FROM_STAGE
# define NEW_FROM_STAGE 1
#endif

#if NEW_FROM_STAGE == 1

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

#elif NEW_FROM_STAGE == 2

#define new_from_(T2, T1)                                               \
    _Generic(*((T2*)(NULL))                                             \
             , SystemError:                                             \
             _Generic(*((T1*)(NULL))                                    \
                      , SystemError: new_SystemError_from_SystemError   \
                      , UnicodeError: /*FAKE*/new_SystemError_from_SystemError \
                      , DecodingError: /*FAKE*/new_SystemError_from_SystemError \
                      , cstr: /*FAKE*/new_SystemError_from_SystemError  \
                      , char*: /*FAKE*/new_SystemError_from_SystemError \
                      , String: /*FAKE*/new_String_from_String          \
                 )                                                      \
             , UnicodeError:                                            \
             _Generic(*((T1*)(NULL))                                    \
                      , UnicodeError: new_UnicodeError_from_UnicodeError \
                      , DecodingError: new_UnicodeError_from_DecodingError \
                      , SystemError: new_UnicodeError_from_SystemError  \
                      , cstr: /*FAKE*/new_SystemError_from_SystemError  \
                      , char*: /*FAKE*/new_SystemError_from_SystemError \
                      , String: /*FAKE*/new_String_from_String          \
                 )                                                      \
             , String:                                                  \
             _Generic(*((T1*)(NULL))                                    \
                      , SystemError: /*FAKE*/new_SystemError_from_SystemError \
                      , UnicodeError: /*FAKE*/new_SystemError_from_SystemError \
                      , DecodingError: /*FAKE*/new_SystemError_from_SystemError \
                      , cstr: new_String_from_cstr                      \
                      , char*: new_String_from_cstr                     \
                      , String: new_String_from_String                  \
                 )                                                      \
        )

#endif

/// Convert `v` into type `T`.

#define new_from(T, v)                          \
    new_from_(T, typeof(v))(v)


#endif
