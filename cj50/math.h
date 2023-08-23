
//! This provides mathematical types: vectors and shapes.

#pragma once

#include <cj50/basic-util.h>
#include <cj50/resret.h>
#include <cj50/Color.h>
#include <cj50/gen/Vec2.h>
#include <cj50/gen/Vec3.h>
#include <cj50/gen/Rect2.h>
#include <cj50/float.h>
#include <cj50/double.h>
#include <cj50/int.h>
#include <SDL2/SDL.h>


#define T int
#include <cj50/gen/template/Vec2.h>
#include <cj50/gen/template/Vec3.h>
#include <cj50/gen/template/Rect2.h>
#undef T

#define T float
#include <cj50/gen/template/Vec2.h>
#include <cj50/gen/template/Vec3.h>
#include <cj50/gen/template/Rect2.h>
#undef T

#define T double
#include <cj50/gen/template/Vec2.h>
#include <cj50/gen/template/Vec3.h>
#include <cj50/gen/template/Rect2.h>
#undef T



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
