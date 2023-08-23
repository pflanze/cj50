#include <cj50/gen/Vec3.h>
#include <cj50/resret.h>


/// A 3-dimensional vector.

/// Because it can contain anything, it is not a Copy type in general
/// and thus its type name starts with an uppercase letter. Most uses
/// will be with number types, though, which are Copy types, and in
/// those cases the Vec3 will be Copy as well.

typedef struct Vec3(T) {
    T x;
    T y;
    T z;
} Vec3(T);

/// Construct a Vec3.
static UNUSED
Vec3(T) XCAT(vec3_, T)(T x, T y, T z) {
    return (Vec3(T)) { x, y, z };
}

static UNUSED
void XCAT(drop_, Vec3(T))(UNUSED Vec3(T) v) {}

static UNUSED
bool XCAT(equal_, Vec3(T))(const Vec3(T) *a, const Vec3(T) *b) {
    return (a->x == b->x) && (a->y == b->y) && (a->z == b->z);
}

static UNUSED
bool XCAT(equal_move_, Vec3(T))(Vec3(T) a, Vec3(T) b) {
    return XCAT(equal_, Vec3(T))(&a, &b);
}

static UNUSED
int XCAT(print_debug_, Vec3(T))(const Vec3(T) *a) {
    INIT_RESRET;
    RESRET(printf("vec3("));
    RESRET(XCAT(print_debug_, T)(&a->x));
    RESRET(printf(", "));
    RESRET(XCAT(print_debug_, T)(&a->y));
    RESRET(printf(", "));
    RESRET(XCAT(print_debug_, T)(&a->z));
    RESRET(printf(")"));
cleanup:
    return ret;
}
static UNUSED
int XCAT(print_debug_move_, Vec3(T))(Vec3(T) a) {
    return XCAT(print_debug_, Vec3(T))(&a);
}


static UNUSED
Vec3(T) XCAT(add_, Vec3(T))(Vec3(T) a, Vec3(T) b) {
    return XCAT(vec3_, T)(a.x + b.x, a.y + b.y, a.z + b.z);
}
static UNUSED
Vec3(T) XCAT(sub_, Vec3(T))(Vec3(T) a, Vec3(T) b) {
    return XCAT(vec3_, T)(a.x - b.x, a.y - b.y, a.z - b.z);
}
static UNUSED
Vec3(T) XCAT(neg_, Vec3(T))(Vec3(T) a) {
    return XCAT(vec3_, T)(-a.x, -a.y, -a.z);
}

static UNUSED
Vec3(T) XCAT(mul_, XCAT(Vec3(T), _float))(Vec3(T) a, float b) {
    return XCAT(vec3_, T)(a.x * b, a.y * b, a.z * b);
}

