
//! This provides mathematical types: vectors and shapes.

#pragma once

#include <cj50/basic-util.h>
#include "cj50/resret.h"
#include <SDL2/SDL.h>


/// A 2-dimentional vector.
typedef struct Vec2 {
    float x;
    float y;
} Vec2;

/// Construct a Vec2.
static UNUSED
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
static UNUSED
Vec3 vec3(float x, float y, float z) {
    return (Vec3) { x, y, z };
}

static UNUSED
Vec2 add_Vec2(Vec2 a, Vec2 b) {
    return vec2(a.x + b.x, a.y + b.y);
}
static UNUSED
Vec2 sub_Vec2(Vec2 a, Vec2 b) {
    return vec2(a.x - b.x, a.y - b.y);
}
static UNUSED
Vec2 neg_Vec2(Vec2 a) {
    return vec2(-a.x, -a.y);
}

static UNUSED
Vec3 add_Vec3(Vec3 a, Vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
static UNUSED
Vec3 sub_Vec3(Vec3 a, Vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
static UNUSED
Vec3 neg_Vec3(Vec3 a) {
    return vec3(-a.x, -a.y, -a.z);
}


static UNUSED
Vec2 mul_Vec2_float(Vec2 a, float b) {
    return vec2(a.x * b, a.y * b);
}
static UNUSED
Vec3 mul_Vec3_float(Vec3 a, float b) {
    return vec3(a.x * b, a.y * b, a.z * b);
}

static UNUSED
int print_debug_Vec2(Vec2 a) {
    return printf("vec2(%g, %g)", a.x, a.y);
}
static UNUSED
int print_debug_Vec3(Vec3 a) {
    return printf("vec3(%g, %g, %g)", a.x, a.y, a.z);
}


/// A line. The end point is at add(start, extent).
typedef struct Line2 {
    Vec2 start;
    Vec2 extent;
} Line2;

/// Construct a Line2
static UNUSED
Line2 line2(Vec2 start, Vec2 extent) {
    return (Line2) { start, extent };
}

static UNUSED
int print_debug_Line2(Line2 s) {
    INIT_RESRET;
    RESRET(printf("Line2("));
    RESRET(print_debug_Vec2(s.start));
    RESRET(printf(", "));
    RESRET(print_debug_Vec2(s.extent));
    RESRET(printf(")"));
cleanup:
    return ret;
}

// Really a copy-paste of Line2, huh!

/// A rectangle. The opposite corner is at add(start, extent).
typedef struct Rect2 {
    Vec2 start;
    Vec2 extent;
} Rect2;


/// Construct a Rect2
static UNUSED
Rect2 rect2(Vec2 start, Vec2 extent) {
    return (Rect2) { start, extent };
}

static UNUSED
int print_debug_Rect2(Rect2 s) {
    INIT_RESRET;
    RESRET(printf("Rect2("));
    RESRET(print_debug_Vec2(s.start));
    RESRET(printf(", "));
    RESRET(print_debug_Vec2(s.extent));
    RESRET(printf(")"));
cleanup:
    return ret;
}


/// A standard 8-bit RGB color representation.
typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

/// Constructing a color
static UNUSED
Color color(uint8_t r, uint8_t g, uint8_t b) {
    return (Color) { r, g, b };
}

static UNUSED
bool equal_Color(const Color *a, const Color *b) {
    return ((a->r == b->r) &&
            (a->g == b->g) &&
            (a->b == b->b));
}

static UNUSED
bool equal_move_Color(Color a, Color b) {
    return equal_Color(&a, &b);
}

static UNUSED
int print_debug_Color(const Color *v) {
    return printf("color(%i, %i, %i)", v->r, v->g, v->b);
}

static UNUSED
int print_debug_move_Color(Color v) {
    return print_debug_Color(&v);
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


/// Add together two values, for which `+` is not defined, e.g. Vec2
/// or Vec3. Both values need to be of the same type.

/// Example:

/// ```C
/// DBG(add(vec2(10, -3), vec2(3, 5)))  /* prints vec2(13, 2) */
/// ```
#define add(a, b)                               \
    _Generic((a)                                \
             , Vec2: add_Vec2                   \
             , Vec3: add_Vec3                   \
        )((a), (b))

/// Subtraction of two values for which `-` is not defined, e.g. Vec2
/// or Vec3. Both values need to be of the same type.

/// Example:

/// ```C
/// DBG(sub(vec2(10, -3), vec2(3, 5)))  /* prints vec2(7, -8) */
/// ```
#define sub(a, b)                               \
    _Generic((a)                                \
             , Vec2: sub_Vec2                   \
             , Vec3: sub_Vec3                   \
        )((a), (b))

/// Negate a value for which `-` is not defined, e.g. Vec2 or Vec3.

/// Example:

/// ```C
/// DBG(neg(vec2(10, -3)))  /* prints vec2(-10, 3) */
/// ```
#define neg(a)                                  \
    _Generic((a)                                \
             , Vec2: neg_Vec2                   \
             , Vec3: neg_Vec3                   \
        )(a)

/// Multiply two values for which `*` is not defined, e.g. Vec2 or
/// Vec3. NOTE: the name of this operation might change! Currently
/// only `float` is supported for the second argument.

/// Example:

/// ```C
/// DBG(mul(vec2(10, -3), 2))  /* prints vec2(20, -6) */
/// ```
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


/// The math constant pi.

/// (This may also be available as `M_PI` from <math.h>.)
const double math_pi = 3.14159265358979323846264338;

// The pi constant as a float value, faster.
const float math_pi_float = 3.14159265358979323846264338f;


/// The math constant e (Euler's number).
const double math_e = 2.71828182845904523536028747;
const double math_e_float = 2.71828182845904523536028747f;
// verified algorithmically: 2.71828182845905


