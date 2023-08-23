#pragma once


/// Add together two values, for which `+` is not defined, e.g. Vec2
/// or Vec3. Both values need to be of the same type.

/// Example:

/// ```C
/// DBG(add(vec2_int(10, -3), vec2_int(3, 5)))  /* prints vec2(13, 2) */
/// ```
#define add(a, b)                                         \
    _Generic((a)                                          \
             , Vec2(int): add_Vec2_int                    \
             , Vec3(int): add_Vec3_int                    \
             , Vec2(float): add_Vec2_float                \
             , Vec3(float): add_Vec3_float                \
             , Vec2(double): add_Vec2_double              \
             , Vec3(double): add_Vec3_double              \
        )((a), (b))

/// Subtraction of two values for which `-` is not defined, e.g. Vec2
/// or Vec3. Both values need to be of the same type.

/// Example:

/// ```C
/// DBG(sub(vec2(10, -3), vec2(3, 5)))  /* prints vec2(7, -8) */
/// ```
#define sub(a, b)                                             \
    _Generic((a)                                              \
             , Vec2(int): sub_Vec2_int                        \
             , Vec3(int): sub_Vec3_int                        \
             , Vec2(float): sub_Vec2_float                    \
             , Vec3(float): sub_Vec3_float                    \
             , Vec2(double): sub_Vec2_double                  \
             , Vec3(double): sub_Vec3_double                  \
        )((a), (b))

/// Negate a value for which `-` is not defined, e.g. Vec2 or Vec3.

/// Example:

/// ```C
/// DBG(neg(vec2(10, -3)))  /* prints vec2(-10, 3) */
/// ```
#define neg(a)                                  \
    _Generic((a)                                \
             , Vec2(int): neg_Vec2_int              \
             , Vec3(int): neg_Vec3_int              \
             , Vec2(float): neg_Vec2_float              \
             , Vec3(float): neg_Vec3_float              \
             , Vec2(double): neg_Vec2_double              \
             , Vec3(double): neg_Vec3_double              \
        )(a)

/// Multiply two values for which `*` is not defined, e.g. Vec2 or
/// Vec3. NOTE: the name of this operation might change! Currently
/// only `float` is supported for the second argument.

/// Example:

/// ```C
/// DBG(mul(vec2(10, -3), 2))  /* prints vec2(20, -6) */
/// ```
#define mul(a, b)                                                   \
    _Generic((a)                                                    \
             , Vec2(int): _Generic((b)                              \
                                   , int: mul_Vec2_int_float        \
                                   , float: mul_Vec2_int_float      \
                                   , double: mul_Vec2_int_float     \
                 )                                                  \
             , Vec3(int): _Generic((b)                              \
                                   , int: mul_Vec3_int_float        \
                                   , float: mul_Vec3_int_float      \
                                   , double: mul_Vec3_int_float     \
                 )                                                  \
             , Vec2(float): _Generic((b)                              \
                                   , int: mul_Vec2_float_float        \
                                   , float: mul_Vec2_float_float      \
                                   , double: mul_Vec2_float_float     \
                 )                                                  \
             , Vec3(float): _Generic((b)                              \
                                   , int: mul_Vec3_float_float        \
                                   , float: mul_Vec3_float_float      \
                                   , double: mul_Vec3_float_float     \
                 )                                                  \
             , Vec2(double): _Generic((b)                              \
                                   , int: mul_Vec2_double_float        \
                                   , float: mul_Vec2_double_float      \
                                   , double: mul_Vec2_double_float     \
                 )                                                  \
             , Vec3(double): _Generic((b)                              \
                                   , int: mul_Vec3_double_float        \
                                   , float: mul_Vec3_double_float      \
                                   , double: mul_Vec3_double_float     \
                 )                                                  \
        )((a), (b))



