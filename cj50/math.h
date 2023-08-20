
//! This provides mathematical types: vectors and shapes.

#pragma once

#include <cj50/basic-util.h>
#include <cj50/resret.h>
#include <cj50/Color.h>
#include <cj50/gen/Vec2.h>
#include <cj50/gen/Vec3.h>
#include <cj50/float.h>
#include <cj50/double.h>
#include <cj50/int.h>
#include <SDL2/SDL.h>


#define T int
#include <cj50/gen/template/Vec2.h>
#include <cj50/gen/template/Vec3.h>
#undef T

#define T float
#include <cj50/gen/template/Vec2.h>
#include <cj50/gen/template/Vec3.h>
#undef T

#define T double
#include <cj50/gen/template/Vec2.h>
#include <cj50/gen/template/Vec3.h>
#undef T



/// A line. The end point is at add(start, extent).
typedef struct Line2 {
    Vec2(float) start;
    Vec2(float) extent;
} Line2;

/// Construct a Line2
static UNUSED
Line2 line2(Vec2(float) start, Vec2(float) extent) {
    return (Line2) { start, extent };
}

static UNUSED
void drop_Line2(UNUSED Line2 v) {}

static UNUSED
int print_debug_Line2(const Line2 *s) {
    INIT_RESRET;
    RESRET(printf("Line2("));
    RESRET(print_debug_move_Vec2_float(s->start));
    RESRET(printf(", "));
    RESRET(print_debug_move_Vec2_float(s->extent));
    RESRET(printf(")"));
cleanup:
    return ret;
}
static UNUSED
int print_debug_move_Line2(Line2 s) {
    return print_debug_Line2(&s);
}

// Really a copy-paste of Line2, huh!

/// A rectangle. The opposite corner is at add(start, extent).
typedef struct Rect2 {
    Vec2(float) start;
    Vec2(float) extent;
} Rect2;


/// Construct a Rect2
static UNUSED
Rect2 rect2(Vec2(float) start, Vec2(float) extent) {
    return (Rect2) { start, extent };
}

static UNUSED
int print_debug_Rect2(const Rect2 *s) {
    INIT_RESRET;
    RESRET(printf("Rect2("));
    RESRET(print_debug_move_Vec2_float(s->start));
    RESRET(printf(", "));
    RESRET(print_debug_move_Vec2_float(s->extent));
    RESRET(printf(")"));
cleanup:
    return ret;
}
static UNUSED
int print_debug_move_Rect2(Rect2 s) {
    return print_debug_Rect2(&s);
    
}


static UNUSED
SDL_FRect to_sdl_Rect2(Rect2 r) {
    return (SDL_FRect) { r.start.x, r.start.y, r.extent.x, r.extent.y };
}


/// The math constant pi.

/// (This may also be available as `M_PI` from <math.h>.)
const double math_pi = 3.14159265358979323846264338;

// The pi constant as a float value, faster.
const float math_pi_float = 3.14159265358979323846264338f;


/// The math constant e (Euler's number).
const double math_e = 2.71828182845904523536028747;
const double math_e_float = 2.71828182845904523536028747f;
// verified algorithmically: 2.71828182845905


/// The square function, generic for all number types for which `*` is
/// defined.
#define square(expr)                            \
    ({                                          \
    typeof(expr) HYGIENIC(var) = (expr);        \
    HYGIENIC(var) * HYGIENIC(var);              \
    })

#include <cj50/gen/dispatch/math.h>
