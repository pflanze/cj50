// parameters: T

#include <cj50/gen/Vec2.h>

/// A 2-dimentional vector.
typedef struct Vec2(T) {
    T x;
    T y;
} Vec2(T);

/// Construct a Vec2.
static UNUSED
Vec2(T) XCAT(vec2_, T)(T x, T y) {
    return (Vec2(T)) { x, y };
}

static UNUSED
void XCAT(drop_, Vec2(T))(UNUSED Vec2(T) v) {}

static UNUSED
bool XCAT(equal_, Vec2(T))(const Vec2(T) *a, const Vec2(T) *b) {
    return (a->x == b->x) && (a->y == b->y);
}

static UNUSED
bool XCAT(equal_move_, Vec2(T))(Vec2(T) a, Vec2(T) b) {
    return XCAT(equal_, Vec2(T))(&a, &b);
}

static UNUSED
int XCAT(print_debug_, Vec2(T))(const Vec2(T) *a) {
    INIT_RESRET;
    RESRET(printf("vec2("));
    RESRET(XCAT(print_debug_, T)(&a->x));
    RESRET(printf(", "));
    RESRET(XCAT(print_debug_, T)(&a->y));
    RESRET(printf(")"));
cleanup:
    return ret;
}
static UNUSED
int XCAT(print_debug_move_, Vec2(T))(Vec2(T) a) {
    return XCAT(print_debug_, Vec2(T))(&a);
}

GENERATE_Option(Vec2(T));


static UNUSED
Vec2(T) XCAT(add_, Vec2(T))(Vec2(T) a, Vec2(T) b) {
    return XCAT(vec2_, T)(a.x + b.x, a.y + b.y);
}
static UNUSED
Vec2(T) XCAT(sub_, Vec2(T))(Vec2(T) a, Vec2(T) b) {
    return XCAT(vec2_, T)(a.x - b.x, a.y - b.y);
}
static UNUSED
Vec2(T) XCAT(neg_, Vec2(T))(Vec2(T) a) {
    return XCAT(vec2_, T)(-a.x, -a.y);
}

static UNUSED
Vec2(T) XCAT(mul_, XCAT(Vec2(T), _float))(Vec2(T) a, float b) {
    return XCAT(vec2_, T)(a.x * b, a.y * b);
}

