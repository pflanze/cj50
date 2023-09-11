// parameters: T

#include <cj50/gen/Rect2.h>
#include <cj50/gen/ref.h>
#include <cj50/resret.h>

/// A 2-dimensional rectangle, made from `start` and the opposite
/// corner which is at `add(start, extent)`.

typedef struct Rect2(T) {
    Vec2(T) start;
    Vec2(T) extent;
} Rect2(T);


/// Construct a Rect2
static UNUSED
Rect2(T) XCAT(rect2_, T)(Vec2(T) start, Vec2(T) extent) {
    return (Rect2(T)) { start, extent };
}

static UNUSED
void XCAT(drop_, Rect2(T))(UNUSED Rect2(T) self) {}

static UNUSED
bool XCAT(equal_, Rect2(T))(const Rect2(T) *a, const Rect2(T) *b) {
    return XCAT(equal_, Vec2(T))(&a->start, &b->start)
        && XCAT(equal_, Vec2(T))(&a->extent, &b->extent);
}

static UNUSED
int XCAT(print_debug_, Rect2(T))(const Rect2(T) *s) {
    INIT_RESRET;
    RESRET(printf("rect2_"));
    RESRET(printf("("));
    RESRET(XCAT(print_debug_move_, Vec2(T))(s->start));
    RESRET(printf(", "));
    RESRET(XCAT(print_debug_move_, Vec2(T))(s->extent));
    RESRET(printf(")"));
cleanup:
    return ret;
}

static UNUSED
int XCAT(print_debug_move_, Rect2(T))(Rect2(T) s) {
    return XCAT(print_debug_, Rect2(T))(&s);
}


GENERATE_Option(Rect2(T));
GENERATE_ref(Rect2(T));
GENERATE_Option(ref(Rect2(T)));
