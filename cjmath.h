#ifndef CJMATH_H_
#define CJMATH_H_


//! This provides mathematical types: vectors and shapes.


#include <u8l/basic-util.h>
#include <SDL2/SDL.h>


#define RESRET(e)                \
    res = (e);                   \
    if (res < 0) { return res; } \
    ret += res;


/// A 2-dimentional vector.
typedef struct Vec2 {
    float x;
    float y;
} Vec2;

/// Construct a Vec2.
Vec2 vec2(float x, float y) {
    return (Vec2) { x, y };
}

/// A 3-dimentional vector.
typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;

/// Construct a Vec3.
Vec3 vec3(float x, float y, float z) {
    return (Vec3) { x, y, z };
}

Vec2 add_Vec2(Vec2 a, Vec2 b) {
    return vec2(a.x + b.x, a.y + b.y);
}
Vec2 sub_Vec2(Vec2 a, Vec2 b) {
    return vec2(a.x - b.x, a.y - b.y);
}
Vec2 neg_Vec2(Vec2 a) {
    return vec2(-a.x, -a.y);
}

Vec3 add_Vec3(Vec3 a, Vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
Vec3 sub_Vec3(Vec3 a, Vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
Vec3 neg_Vec3(Vec3 a) {
    return vec3(-a.x, -a.y, -a.z);
}


Vec2 mul_Vec2_float(Vec2 a, float b) {
    return vec2(a.x * b, a.y * b);
}
Vec3 mul_Vec3_float(Vec3 a, float b) {
    return vec3(a.x * b, a.y * b, a.z * b);
}

int print_debug_Vec2(Vec2 a) {
    return printf("Vec2(%g, %g)", a.x, a.y);
}
int print_debug_Vec3(Vec3 a) {
    return printf("Vec3(%g, %g, %g)", a.x, a.y, a.z);
}


/// A line. The end point is at add(start, extent).
typedef struct Line2 {
    Vec2 start;
    Vec2 extent;
} Line2;

#define Line2(start, extent) ((Line2) { (start), (extent) })

int print_debug_Line2(Line2 s) {
    int ret = 0;
    int res;
    RESRET(printf("Line2("));
    RESRET(print_debug_Vec2(s.start));
    RESRET(printf(", "));
    RESRET(print_debug_Vec2(s.extent));
    RESRET(printf(")"));
    return ret;
}

// Really a copy-paste of Line2, huh!

/// A rectangle. The opposite corner is at add(start, extent).
typedef struct Rect2 {
    Vec2 start;
    Vec2 extent;
} Rect2;

#define Rect2(start, extent) ((Rect2) { (start), (extent) })

int print_debug_Rect2(Rect2 s) {
    int ret = 0;
    int res;
    RESRET(printf("Rect2("));
    RESRET(print_debug_Vec2(s.start));
    RESRET(printf(", "));
    RESRET(print_debug_Vec2(s.extent));
    RESRET(printf(")"));
    return ret;
}



static UNUSED
SDL_Rect to_sdl_Rect2(Rect2 r) {
    return (SDL_Rect) { r.start.x, r.start.y, r.extent.x, r.extent.y };
}


/// Convert a value of a type from cjmath.h into a type from SDL.h
#define to_sdl(v)                               \
    _Generic((v)                                \
             , Rect2: to_sdl_Rect2              \
        )(v)


/// Add two values, for which `+` is not defined, together, e.g. Vec2
/// or Vec3. Both values need to be of the same type.
#define add(a, b)                               \
    _Generic((a)                                \
             , Vec2: add_Vec2                   \
             , Vec3: add_Vec3                   \
        )((a), (b))

/// Subtraction of two values for which `-` is not defined, e.g. Vec2
/// or Vec3. Both values need to be of the same type.
#define sub(a, b)                               \
    _Generic((a)                                \
             , Vec2: sub_Vec2                   \
             , Vec3: sub_Vec3                   \
        )((a), (b))

/// Negate a value for which `-` is not defined, e.g. Vec2 or Vec3.
#define neg(a)                                  \
    _Generic((a)                                \
             , Vec2: neg_Vec2                   \
             , Vec3: neg_Vec3                   \
        )(a)

/// Multiply two values for which `*` is not defined, e.g. Vec2 or
/// Vec3. Both values need to be of the same type.
#define mul(a, b)                                               \
    _Generic((a)                                                \
             , Vec2: _Generic((b)                               \
                              , int: mul_Vec2_float             \
                              , float: mul_Vec2_float           \
                              , double: mul_Vec2_float          \
                 )                                              \
             , Vec3: _Generic((b)                               \
                              , int: mul_Vec3_float             \
                              , float: mul_Vec3_float           \
                              , double: mul_Vec3_float          \
                 )                                              \
        )((a), (b))


#undef RESRET

#endif /* CJMATH_H_ */
